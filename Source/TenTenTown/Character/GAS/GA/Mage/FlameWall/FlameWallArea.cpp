#include "FlameWallArea.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

AFlameWallArea::AFlameWallArea()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	PrimaryActorTick.bCanEverTick = false;
}

void AFlameWallArea::Init(float InLifeTime)
{
	Lifetime = InLifeTime;
}

void AFlameWallArea::OnDamageZoneBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor || !DotGE) return;
	if (OtherActor == GetOwner()) return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);

	if (!SourceASC || !TargetASC) return;
	
	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	
	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DotGE, 1.f, Ctx);
	if (!Spec.IsValid()) return;

	Spec.Data->SetSetByCallerMagnitude(Tag_DoT, -DamagePerTick);
	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
}

void AFlameWallArea::OnDamageZoneEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority() || !OtherActor) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (!TargetASC) return;

	if (!DotGrantedTags.IsEmpty())
	{
		TargetASC->RemoveActiveEffectsWithGrantedTags(DotGrantedTags);
	}
}

void AFlameWallArea::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetLifeSpan(Lifetime);
	}
}

