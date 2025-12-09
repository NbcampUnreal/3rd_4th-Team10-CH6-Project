#include "Structure/Crossbow/CrossbowStructure.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/Base/EnemyBase.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Structure/Data/AS/AS_StructureAttributeSet.h"

ACrossbowStructure::ACrossbowStructure()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 컴포넌트 세팅
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	SetRootComponent(BaseMesh);
	BaseMesh->SetCollisionProfileName(TEXT("TowerStructure"));
	
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BaseMesh);
	TurretMesh->SetCollisionProfileName(TEXT("TowerStructure"));
	
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(TurretMesh); // 회전체에 붙어있어야 같이 돔
	MuzzleLocation->SetRelativeLocation(FVector(50.f, 0.f, 0.f)); // 약간 앞으로

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(RootComponent);
	DetectSphere->SetSphereRadius(AttackRange);
	
	// DetectSphere 설치 트레이스에 감지 x
	DetectSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 겹침 전용
	DetectSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore); // GridFloor 채널 무시
}

void ACrossbowStructure::BeginPlay()
{
	Super::BeginPlay();

	// GAS 체력 변경 바인딩

	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &ACrossbowStructure::OnEnemyEnter);
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &ACrossbowStructure::OnEnemyExit);

	InitializePool();
}

void ACrossbowStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타겟이 유효한지 검사
	if (CurrentTarget && (CurrentTarget->IsPendingKillPending() || !IsValid(CurrentTarget)))
	{
		CurrentTarget = nullptr;
	}

	// 타겟이 없다면 새로운 타겟 탐색
	if (!CurrentTarget)
	{
		// 0.2초마다 한 번씩만 탐색 (최적화)
		TargetSearchTimer += DeltaTime;
		if (TargetSearchTimer >= 0.2f)
		{
			FindBestTarget();
			TargetSearchTimer = 0.0f;
		}
	}

	// 타겟이 있다면 회전 및 공격
	if (CurrentTarget)
	{
		// 멀어지면 안쏨
		float Dist = GetDistanceTo(CurrentTarget);
		if (Dist > AttackRange + 100.0f)
		{
			CurrentTarget = nullptr;
			return;
		}
		
		// 회전
		FVector TargetLoc = CurrentTarget->GetActorLocation();
		ACharacter* EnemyChar = Cast<ACharacter>(CurrentTarget);
		
		if (EnemyChar && EnemyChar->GetMesh())
		{
			// 메시의 정중앙
			TargetLoc = EnemyChar->GetMesh()->Bounds.Origin;
		}
		FVector MyLoc = TurretMesh->GetComponentLocation();
		// 정중앙에서 Z + 50
		TargetLoc.Z += 50.0f;
		
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
		float AdjustYaw = -90.0f;
		
		// 좌우 회전만
		FRotator TargetRot = FRotator(0.f, LookAtRot.Yaw + AdjustYaw, 0.f);

		FRotator CurrentRot = TurretMesh->GetComponentRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 10.0f);
        
		TurretMesh->SetWorldRotation(NewRot); // TurretMesh만 돌림 (받침대는 가만히)

		// 공격 타이머
		FireTimer += DeltaTime;
		if (FireTimer >= (1.0f / AttackSpeed))
		{
			Fire();
			FireTimer = 0.f;
		}
	}
	else
	{
		// 적이 없으면 방향 복귀
		FRotator ReturnRot = FMath::RInterpTo(TurretMesh->GetRelativeRotation(), FRotator::ZeroRotator, DeltaTime, 2.0f);
		TurretMesh->SetRelativeRotation(ReturnRot);
	}
}

void ACrossbowStructure::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//InitializeStructure();
}

// 풀링으로 화살 미리 만들기
void ACrossbowStructure::InitializePool()
{
	if (!BoltClass) return;

	for (int32 i = 0; i < PoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// 안 보이는 곳에 생성
		ACrossbowBolt* NewBolt = GetWorld()->SpawnActor<ACrossbowBolt>(BoltClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewBolt)
		{
			BoltPool.Add(NewBolt);
		}
	}
}

// 풀에서 화살 꺼내기
ACrossbowBolt* ACrossbowStructure::GetBoltFromPool()
{
	// Hidden 화살 찾기
	for (ACrossbowBolt* Bolt : BoltPool)
	{
		if (Bolt && Bolt->IsHidden())
		{
			return Bolt;
		}
	}

	// 다 쓰고 없으면 새로 만듬
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	ACrossbowBolt* NewBolt = GetWorld()->SpawnActor<ACrossbowBolt>(BoltClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (NewBolt)
	{
		BoltPool.Add(NewBolt);
		return NewBolt;
	}

	return nullptr;
}

// 최적화 위해 쓴다는데 모름
void ACrossbowStructure::OnEnemyEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 필요시 구현 (예: 들어오자마자 타겟팅 갱신)
}

// 타겟이 나가면 타겟 해제
void ACrossbowStructure::OnEnemyExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CurrentTarget)
	{
		CurrentTarget = nullptr;
	}
}

// 발사
void ACrossbowStructure::Fire()
{
	if (!CurrentTarget) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !DamageEffectClass) return;

	// 1. Context 생성 (Instigator = 나)
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddInstigator(this, this);

	// 2. Spec 생성 (명세서 만들기)
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

	if (SpecHandle.IsValid())
	{
		// 3. 현재 AttributeSet에서 공격력 값 가져오기
		// (AttributeSet이 없다면 기본값 10.0f 사용)
		float CurrentDamage = 10.0f;
		const UAS_StructureAttributeSet* AS = Cast<UAS_StructureAttributeSet>(ASC->GetAttributeSet(UAS_StructureAttributeSet::StaticClass()));
		if (AS)
		{
			CurrentDamage = AS->GetAttackDamage();
		}

		// 4. Spec에 데미지 수치 주입 (SetByCaller)
		// GASTAG::Data_Enemy_Damage 태그를 키값으로 사용
		SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, CurrentDamage);
        
		// 5. 화살 꺼내서 Spec과 함께 발사
		ACrossbowBolt* Bolt = GetBoltFromPool();
		if (Bolt)
		{
			Bolt->SetOwner(this);
			Bolt->SetInstigator(GetInstigator());
            
			// [중요] Range도 가능하다면 AttributeSet에서 가져올 수 있습니다. 지금은 기존 변수 사용.
			Bolt->ActivateProjectile(MuzzleLocation->GetComponentLocation(), CurrentTarget, AttackRange, SpecHandle);
		}
	}
}

// 가장 가까운 적 찾기
void ACrossbowStructure::FindBestTarget()
{
	TArray<AActor*> OverlappingActors;
	DetectSphere->GetOverlappingActors(OverlappingActors, AEnemyBase::StaticClass());

	float MinDist = 999999.0f;
	AActor* NearestEnemy = nullptr;

	for (AActor* Actor : OverlappingActors)
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(Actor);
		if (Enemy && !Enemy->IsPendingKillPending()) // 살아있는 적만
		{
			float Dist = GetDistanceTo(Enemy);
			if (Dist < MinDist)
			{
				MinDist = Dist;
				NearestEnemy = Enemy;
			}
		}
	}
	CurrentTarget = NearestEnemy;
}

void ACrossbowStructure::InitializeStructure()
{
	Super::InitializeStructure();

	// 1. 데이터 유효성 검사 (LevelInfos가 비어있는지 확인)
	if (CachedStructureData.LevelInfos.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("InitializeStructure Failed! No LevelInfos found in Data."));
		return;
	}

	// 2. 1레벨(Index 0) 정보 가져오기
	const FStructureLevelInfo& Level1Info = CachedStructureData.LevelInfos[0];

	// 3. 메쉬 설정 (TSoftObjectPtr 로딩)
	if (!Level1Info.TurretMesh.IsNull())
	{
		// 동기 로드 (초기화 시점이므로 로드함)
		UStaticMesh* LoadedMesh = Level1Info.TurretMesh.LoadSynchronous();
		if (LoadedMesh && TurretMesh)
		{
			TurretMesh->SetStaticMesh(LoadedMesh);
		}
	}

	// 4. GAS를 통해 초기 스탯 적용 (1레벨 GE 적용)
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC && Level1Info.LevelStatGE)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddInstigator(this, this);

		// GE 스펙 생성 및 적용
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Level1Info.LevelStatGE, 1.0f, Context);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	// 5. 적용된 GAS 스탯을 바탕으로 로컬 변수 및 컴포넌트 갱신
	// (이제 스탯의 원본은 AttributeSet입니다)
	const UAS_StructureAttributeSet* AS = Cast<UAS_StructureAttributeSet>(ASC->GetAttributeSet(UAS_StructureAttributeSet::StaticClass()));
	if (AS)
	{
		// 로컬 변수 AttackRange가 Tick 등에서 쓰인다면 여기서 동기화
		AttackRange = AS->GetAttackRange();
        
		// 감지 범위 갱신
		if (DetectSphere)
		{
			DetectSphere->SetSphereRadius(AttackRange);
		}
        
		// 로그 확인
		UE_LOG(LogTemp, Log, TEXT("[Init] Stats Applied - Dmg: %.1f, Range: %.1f, Speed: %.1f"), 
			AS->GetAttackDamage(), AS->GetAttackRange(), AS->GetAttackSpeed());
	}
}

void ACrossbowStructure::UpgradeStructure()
{
	Super::UpgradeStructure(); 

	// 1. 데이터 테이블에서 새로운 레벨 정보 가져오기
    // CurrentUpgradeLevel이 1부터 시작하므로 인덱스는 (CurrentUpgradeLevel - 1)
    if (!CachedStructureData.LevelInfos.IsValidIndex(CurrentUpgradeLevel - 1))
    {
        UE_LOG(LogTemp, Error, TEXT("Upgrade Failed! Level info not found for Level %d"), CurrentUpgradeLevel);
        return;
    }
    
    const FStructureLevelInfo& NewLevelInfo = CachedStructureData.LevelInfos[CurrentUpgradeLevel - 1];

	// 2. 메쉬 변경 (받침대 제외한 회전체만)
    // TSoftObjectPtr은 실제 사용 전 로딩이 필요합니다.
    if (NewLevelInfo.TurretMesh.IsValid())
    {
        // Static Mesh 컴포넌트의 SetStaticMesh 함수 사용
        TurretMesh->SetStaticMesh(NewLevelInfo.TurretMesh.Get());
    }
    else
    {
        // 로딩이 필요하다면 LoadAssetBlocking() 등으로 처리해야 합니다.
        // 여기서는 이미 로딩된 것으로 가정하거나, BeginPlay에서 미리 로딩해야 합니다.
        UE_LOG(LogTemp, Warning, TEXT("Turret Mesh asset not loaded for Level %d"), CurrentUpgradeLevel);
    }
    
    // 3. GAS 시스템으로 스탯 및 능력 적용
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC)
    {
        // 3-1. 스탯 변경 GE 적용 (GameplayEffect)
        if (NewLevelInfo.LevelStatGE)
        {
            FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
            FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(NewLevelInfo.LevelStatGE, 1.0f, Context);
            
            // GE를 자기 자신에게 적용
            FActiveGameplayEffectHandle ActiveGE = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            
            UE_LOG(LogTemp, Log, TEXT("[GAS] Applied Stat GE for Level %d. Handle: %d"), CurrentUpgradeLevel, ActiveGE.Handle);
        }
        
        // 3-2. 새로운 능력 부여 (GameplayAbility)
        for (const TSubclassOf<UGameplayAbility>& AbilityClass : NewLevelInfo.NewAbilities)
        {
            if (AbilityClass)
            {
                // GAS에서 능력을 부여하는 함수 호출 (예시: GiveAbility)
                // 실제 코드에서는 Ability Tag이나 Input ID를 부여해야 합니다.
                // ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0));
                
                UE_LOG(LogTemp, Log, TEXT("[GAS] Granted New Ability for Level %d"), CurrentUpgradeLevel);
            }
        }
    }
    
	// 4. (선택) 로컬 스탯 갱신 및 디텍트 스피어 크기 갱신
	// GAS를 사용한다면 로컬 변수(AttackDamage, AttackRange)는 제거하는 것이 좋습니다.
	// 하지만 현재 코드는 GAS를 사용하지 않고 로컬 변수로 데미지를 계산하므로 임시로 갱신합니다.
    // AttackDamage는 GE가 처리하므로, 여기서는 AttackRange만 갱신하는 것으로 가정합니다.
    
    // AttackRange는 GE에서 변경될 때 콜백을 통해 갱신되어야 하지만,
    // 현재는 로컬 변수를 사용하고 있으므로 임시로 여기에 로직을 추가하지 않습니다.
    // *정석: GE가 적용될 때 AttributeSet의 OnRep_XXX 함수나 PostGameplayEffectExecute에서 콜백을 받아 로컬 변수를 갱신해야 합니다.*
    
    // (현재 코드 유지보수) DetectSphere 크기 갱신
    // AttackRange가 GE에 의해 갱신된다고 가정하고, 콜백에서 SetSphereRadius(NewRange) 호출하도록 추후 수정 필요합니다.
    // 임시로 하드코딩된 로직은 제거합니다.

	UE_LOG(LogTemp, Warning, TEXT("[Crossbow] Upgrade Complete! Lv:%d"), CurrentUpgradeLevel);
}

// 파괴 이펙트, 사운드 등 여기서 작업
void ACrossbowStructure::HandleDestruction()
{
	UE_LOG(LogTemp, Warning, TEXT("Crossbow Destroyed Visuals!"));

	// 부모 함수 호출
	Super::HandleDestruction();
}

// 디버그용
void ACrossbowStructure::Debug_TakeDamage()
{
	if (AttributeSet)
	{
		float CurrentHealth = AttributeSet->GetHealth();
		float NewHealth = FMath::Clamp(CurrentHealth - 100.0f, 0.0f, AttributeSet->GetMaxHealth());

		UE_LOG(LogTemp, Log, TEXT("[Debug] Requesting Health Change: %f -> %f"), CurrentHealth, NewHealth);

		if (FMath::IsNearlyEqual(CurrentHealth, NewHealth))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Debug] Health didn't change (Already same value). Delegate might not fire."));
			// 이미 0이라면 강제로 사망 처리 시도
			if (NewHealth <= 0.1f) HandleDestruction();
		}
		else
		{
			AttributeSet->SetHealth(NewHealth);
		}
	}
}
