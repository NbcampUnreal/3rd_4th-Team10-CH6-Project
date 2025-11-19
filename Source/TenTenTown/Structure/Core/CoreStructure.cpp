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
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// 기본 컴포넌트
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	DetectCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectCollision"));
	DetectCollision->SetupAttachment(RootComponent);
	DetectCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// GAS 컴포넌트
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // 안 움직이니까

	AttributeSet = CreateDefaultSubobject<UAS_CoreAttributeSet>(TEXT("AttributeSet"));
}

float ACoreStructure::GetCurrentHealth() const
{
	return AttributeSet ? AttributeSet->GetHealth() : 0.0f;
}

float ACoreStructure::GetMaxHealth() const
{
	return AttributeSet ? AttributeSet->GetMaxHealth() : 0.0f;
}

void ACoreStructure::BeginPlay()
{
	Super::BeginPlay();

	// ASC 초기화
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
	}
	// 체력 변화 감지 연결
	if (AttributeSet)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UAS_CoreAttributeSet::GetHealthAttribute())
			.AddUObject(this, &ACoreStructure::OnHealthAttributeChanged);
	}
	// 서버에서만 오버랩 이벤트
	if (GetLocalRole() == ROLE_Authority)
	{
		DetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ACoreStructure::OnCoreOverlap);
	}
}

void ACoreStructure::OnCoreOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 오버랩된 Actor가 AEnemyBase인지 확인
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCoreOverlap - 몬스터가 아님 (Cast to AEnemyBase 실패)"));
		return; // 몬스터가 아니면 무시
	}
	// 루트컴포 아니면 무시
	if (OtherComp != Enemy->GetRootComponent())
	{
		return;
	}
	// 사라지는 중에 부딪히는 것 방지
	if (Enemy->GetLifeSpan() > 0.0f)
	{
		return;
	}
	// GE 설정 확인
	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ACoreBuilding::DamageEffectClass가 설정되지 않았습니다!"));
		return;
	}
	
	// 코어 공격력 가져오기(추후 몬스터 별 코어 공격력 데이터셋 필요)
	const float DamageToDeal = 1.f;
	UE_LOG(LogTemp, Log, TEXT("OnCoreOverlap - %s가 %f의 데미지를 주려고 시도합니다."), *Enemy->GetName(), DamageToDeal);

	// 데미지 전달 Spec
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, ASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		// 데미지 값 설정
		SpecHandle.Data->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
			-DamageToDeal
		);

		// 데미지 값 코어에 적용
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}

	// 몬스터 제거
	UE_LOG(LogTemp, Warning, TEXT("OnCoreOverlap - 몬스터(%s)를 제거합니다."), *Enemy->GetName());
	Enemy->SetLifeSpan(0.1f);
}

void ACoreStructure::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;
	float MaxHealth = GetMaxHealth();

	OnHPChanged.Broadcast(NewHealth, MaxHealth);

	if (NewHealth <= 0.0f)
	{
		OnDead.Broadcast();
		// 콜리전 해제
		DetectCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
