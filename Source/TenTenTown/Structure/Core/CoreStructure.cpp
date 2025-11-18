#include "Structure/Core/CoreStructure.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagsManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Structure/Core/AS/AS_CoreAttributeSet.h"

ACoreStructure::ACoreStructure()
{
	PrimaryActorTick.bCanEverTick = true;

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // 코어 건물도 복제되어야 합니다.

	// 기본 컴포넌트 생성
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	DetectCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectCollision"));
	DetectCollision->SetupAttachment(RootComponent);
	DetectCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 오버랩 이벤트 발생 설정

	// GAS 컴포넌트 생성
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // 코어는 자주 움직이지 않으므로 Minimal 추천

	AttributeSet = CreateDefaultSubobject<UAS_CoreAttributeSet>(TEXT("AttributeSet"));
}

void ACoreStructure::BeginPlay()
{
	Super::BeginPlay();

	// ASC 초기화
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
	}

	// 서버에서만 오버랩 이벤트를 바인딩합니다. (로직은 서버에서만 처리)
	if (GetLocalRole() == ROLE_Authority)
	{
		DetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ACoreStructure::OnCoreOverlap);
	}
}

void ACoreStructure::OnCoreOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCoreOverlap - 함수 실행됨. Overlapped Actor: %s"), *OtherActor->GetName());
	
	// 1. 오버랩된 Actor가 몬스터(AEnemyBase)인지 확인
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCoreOverlap - 몬스터가 아님 (Cast to AEnemyBase 실패)"));
		return; // 몬스터가 아니면 무시
	}

	UE_LOG(LogTemp, Warning, TEXT("[로그 3] OnCoreOverlap - 몬스터 캐스팅 성공."));
	
	// 2. 데미지를 적용할 GameplayEffect가 설정되었는지 확인
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoreBuilding::DamageEffectClass가 설정되지 않았습니다!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[로그 5] OnCoreOverlap - DamageEffectClass가 ' %s ' (으)로 설정됨."), *DamageEffectClass->GetName());
	
	// 3. 몬스터의 AttributeSet에서 '공격력' (코어에 가할 데미지) 가져오기
	const float DamageToDeal = 1.f;
	UE_LOG(LogTemp, Log, TEXT("OnCoreOverlap - %s가 %f의 데미지를 주려고 시도합니다."), *Enemy->GetName(), DamageToDeal);

	// 4. GameplayEffectSpec을 생성하여 데미지 값(SetByCaller) 전달
	// Spec: GE를 실행하기 위한 모든 정보가 담긴 '설계도'
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		// "SetByCaller" 방식을 사용해 GE에 동적으로 데미지 값을 설정합니다.
		// "Data.Damage" 태그는 우리가 임의로 정한 태그입니다. (3단계에서 사용)
		SpecHandle.Data->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
			-DamageToDeal
		);

		// 5. 생성된 Spec을 자신(코어)에게 적용
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnCoreOverlap - 몬스터(%s)를 제거합니다."), *Enemy->GetName());
	// 6. 몬스터 제거 (DeadTask.cpp에서처럼 SetLifeSpan 사용)
	Enemy->SetLifeSpan(0.1f);
}
