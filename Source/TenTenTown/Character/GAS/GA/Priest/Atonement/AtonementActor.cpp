#include "AtonementActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Enemy/Base/EnemyBase.h"

AAtonementActor::AAtonementActor()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	AreaVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	AreaVFX->SetupAttachment(Root);
	AreaVFX->SetAutoActivate(true);
	
	Area = CreateDefaultSubobject<USphereComponent>(TEXT("AtonementArea"));
	Area->SetupAttachment(Root);
	Area->SetCollisionProfileName(TEXT("AoE"));
	Area->SetGenerateOverlapEvents(true);
	Area->SetSphereRadius(AoERadius);
	
	Area->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAreaBeginOverlap);
	Area->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnAreaEndOverlap);
	
	PrimaryActorTick.bCanEverTick = false;
}

void AAtonementActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) return;
	
	if (AoELifeTime > 0.f)
	{
		SetLifeSpan(AoELifeTime);
	}

	if (HasAuthority() && SpeedUpGE && SpeedUpRefreshInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			SpeedUpRefreshTimerHandle,
			this,
			&ThisClass::RefreshSpeedUpBuffs,
			SpeedUpRefreshInterval,
			true
		);
	}
}

void AAtonementActor::OnAreaBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this) return;
	
	ABaseCharacter* Char = Cast<ABaseCharacter>(OtherActor);
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (!Char && !Enemy) return;
	
	const FVector SourceLoc = GetActorLocation();
	const FVector TargetLoc = OtherActor->GetActorLocation();

	const float ZDiff = FMath::Abs(TargetLoc.Z - SourceLoc.Z);
	if (ZDiff > AoEHalfHeight) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (!ASC) return;

	if (Char)
	{
		if (ShieldGE && !AlreadyShieldedChars.Contains(Char))
		{
			ApplyGEToASC(ASC, ShieldGE, 1.f, ShieldTag, ShieldAmount);
			ApplyGEToASC(ASC, ShieldActiveGE, 1.f, ShieldActiveTag, 0);
			AlreadyShieldedChars.Add(Char);
		}

		CharsInArea.Add(Char);
		if (SpeedUpGE)
		{
			ApplyGEToASC(ASC, SpeedUpGE, 1.f, SpeedUpTag, SpeedUpRate);
		}
	}

	if (Enemy)
	{
		if (SlowGE)
		{
			ApplyGEToASC(ASC, SlowGE, 1.f, SlowTag, SlowRate);
		}
		if (VulnGE)
		{
			ApplyGEToASC(ASC, VulnGE, 1.f, VulnTag, VulnerabilityRate);
		}
	}
}

void AAtonementActor::OnAreaEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this) return;
	
	ABaseCharacter* Char = Cast<ABaseCharacter>(OtherActor);
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (!Char && !Enemy) return;
	
	CharsInArea.Remove(Char);
}

void AAtonementActor::ApplyGEToASC(
	UAbilitySystemComponent* TargetASC,
	TSubclassOf<UGameplayEffect> GEClass,
	float Level,
	FGameplayTag  SetByCallerTag,
	float SetByCallerValue) const
{
	if (!TargetASC || !*GEClass) return;
	
	FGameplayEffectContextHandle Ctx = TargetASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GEClass, Level, Ctx);
	if (SpecHandle.IsValid())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (Spec && SetByCallerTag.IsValid())
		{
			Spec->SetSetByCallerMagnitude(SetByCallerTag, SetByCallerValue);
		}
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AAtonementActor::RefreshSpeedUpBuffs()
{
	if (!HasAuthority() || !SpeedUpGE) return;

	for (auto It = CharsInArea.CreateIterator(); It; ++It)
	{
		TWeakObjectPtr<ABaseCharacter> WeakChar = *It;
		if (!WeakChar.IsValid())
		{
			It.RemoveCurrent();
			continue;
		}

		ABaseCharacter* Char = WeakChar.Get();
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Char);
		if (!ASC) continue;

		ApplyGEToASC(ASC, SpeedUpGE, 1.f, SpeedUpTag, SpeedUpRate);
	}
}
