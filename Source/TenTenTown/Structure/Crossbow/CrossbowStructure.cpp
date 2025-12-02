#include "Structure/Crossbow/CrossbowStructure.h"
#include "Structure/Data/AS/AS_StructureAttributeSet.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/Base/EnemyBase.h"
#include "Engine/World.h"
#include "Engine/CollisionProfile.h"
#include "AbilitySystemComponent.h"

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

	// --- [GAS] 컴포넌트 생성 ---
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAS_StructureAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ACrossbowStructure::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACrossbowStructure::BeginPlay()
{
	Super::BeginPlay();

	// [GAS] ASC 초기화
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// [GAS] 체력 변경 감지
		if (AttributeSet)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				UAS_StructureAttributeSet::GetHealthAttribute()
			).AddUObject(this, &ACrossbowStructure::OnHealthChanged);
		}
	}
	
	// DT 적용
	RefreshStatus();

	// 이벤트 바인딩
	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &ACrossbowStructure::OnEnemyEnter);
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &ACrossbowStructure::OnEnemyExit);

	// 오브젝트 풀 생성
	InitializePool();
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

	RefreshStatus();
}

void ACrossbowStructure::RefreshStatus()
{
	if (!StructureDataTable || StructureRowName.IsNone()) return;

	// 데이터 찾기
	static const FString ContextString(TEXT("Structure Data Context"));
	FStructureData* Data = StructureDataTable->FindRow<FStructureData>(StructureRowName, ContextString);

	if (Data)
	{
		// 스탯 덮어씌우기
		AttackDamage = Data->AttackDamage;
		AttackSpeed = Data->AttackSpeed;
		AttackRange = Data->AttackRange;

		if (DetectSphere)
		{
			DetectSphere->SetSphereRadius(AttackRange);
		}

		// GAS 체력 적용
		if (AttributeSet && GetWorld() && (GetWorld()->IsGameWorld()))
		{
			if (HasAuthority())
			{
				AttributeSet->SetMaxHealth(Data->Health);
				AttributeSet->SetHealth(Data->Health);
			}
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

void ACrossbowStructure::InitializeStructure(const FStructureData& Data)
{
	AttackDamage = Data.AttackDamage;
	AttackSpeed = Data.AttackSpeed;
	AttackRange = Data.AttackRange;

	// 사거리 변경사항 컴포넌트에 즉시 적용
	if (DetectSphere)
	{
		DetectSphere->SetSphereRadius(AttackRange);
	}

	// GAS 체력 업데이트
	if (AttributeSet && HasAuthority())
	{
		if (HasAuthority()) 
		{
			AttributeSet->SetMaxHealth(Data.Health);
			AttributeSet->SetHealth(Data.Health);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Structure Initialized: HP %f/%f, Dmg %f"), Data.Health, Data.Health, Data.AttackDamage);
}

void ACrossbowStructure::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;
	float OldHealth = Data.OldValue;

	UE_LOG(LogTemp, Warning, TEXT("[GAS] OnHealthChanged: %.1f -> %.1f"), OldHealth, NewHealth);
	// 데미지를 입었을 때
	if (NewHealth < OldHealth)
	{
		// 피격 이펙트 추가 예정
	}

	// 체력이 0 이하면 파괴
	if (NewHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS] Health is Zero! Calling HandleDestruction."));
		HandleDestruction();
	}
}

// 파괴 이펙트, 사운드 등 여기서 작업
void ACrossbowStructure::HandleDestruction()
{
	// 파괴 예정이면 무시
	if (IsPendingKillPending()) return;
	
	if (HasAuthority())
	{
		Destroy();
	}
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

// 발사
void ACrossbowStructure::Fire()
{
	if (!CurrentTarget) return;

	ACrossbowBolt* Bolt = GetBoltFromPool();
	if (Bolt)
	{
		Bolt->SetOwner(this);
		Bolt->SetInstigator(GetInstigator());
		Bolt->DamageAmount = this->AttackDamage;
		Bolt->ActivateProjectile(MuzzleLocation->GetComponentLocation(), CurrentTarget, AttackRange);
	}
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
