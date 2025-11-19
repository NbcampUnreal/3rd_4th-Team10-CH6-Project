#include "FlameThrowerActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CollisionQueryParams.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "GameplayEffect.h"

AFlameThrowerActor::AFlameThrowerActor()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	VFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	VFX->SetupAttachment(Root);
	VFX->SetAutoActivate(false);

	DamageZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageZone"));
	DamageZone->SetupAttachment(Root);
	DamageZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageZone->SetGenerateOverlapEvents(true);
	DamageZone->SetCollisionObjectType(ECC_WorldDynamic);
	DamageZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	DamageZone->OnComponentBeginOverlap.AddDynamic(this, &AFlameThrowerActor::OnDamageZoneBeginOverlap);
	DamageZone->OnComponentEndOverlap.AddDynamic(this, &AFlameThrowerActor::OnDamageZoneEndOverlap);

	PrimaryActorTick.bCanEverTick = false;
}

void AFlameThrowerActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlameThrowerActor, Range);
	DOREPLIFETIME(AFlameThrowerActor, ConeHalfAngleDeg);
	DOREPLIFETIME(AFlameThrowerActor, bFiring);  
}

void AFlameThrowerActor::OnDamageZoneBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
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

	if (OtherActor->ActorHasTag(TEXT("Playable"))) return;

	BurningEnemies.Add(OtherActor);
}

void AFlameThrowerActor::OnDamageZoneEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority() || !OtherActor) return;

	BurningEnemies.Remove(OtherActor);
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (!TargetASC) return;

	if (!DotGrantedTags.IsEmpty())
	{
		TargetASC->RemoveActiveEffectsWithGrantedTags(DotGrantedTags);
	}
}

void AFlameThrowerActor::Init(float InInterval, float InConeHalfAngleDeg, float InMaxChannelTime)
{
	if(!HasAuthority()) return;
	
	OwnerChar = Cast<ACharacter>(GetOwner());
	TickInterval = InInterval;
	ConeHalfAngleDeg = InConeHalfAngleDeg;

	if (OwnerChar.IsValid())
	{
		DamageZone->IgnoreActorWhenMoving(OwnerChar.Get(), true);
	}
	UpdateDamageZoneTransform();
	
	if (InMaxChannelTime > 0.f)
	{
		SetLifeSpan(InMaxChannelTime + 0.2);
	}

	if (VFX)
	{
		VFX->Activate(true);
	}

	bFiring = true;
	OnRep_Firing();
	
	GetWorldTimerManager().SetTimer(
		TickTimer,
		this,
		&AFlameThrowerActor::ServerTickDamage,
		TickInterval,
		true,
		0.f
	);
}

void AFlameThrowerActor::OnRep_Firing()
{
	if (VFX)
	{
		if (bFiring)
			VFX->Activate(true);
		else
			VFX->DeactivateImmediate();
	}

}

void AFlameThrowerActor::ServerTickDamage()
{
	if (!HasAuthority() || !OwnerChar.IsValid()) return;

	FVector Start, Dir;
	if (!GetStartAndDir(Start, Dir)) return;

	SetActorLocationAndRotation(Start, Dir.Rotation(), false, nullptr, ETeleportType::TeleportPhysics);
	UpdateDamageZoneTransform();

	OverheatTimeAcc += TickInterval;
	if (OverheatTimeAcc >= OverheatStackInterval)
	{
		OverheatTimeAcc = 0.f;
		
		if (BurningEnemies.Num() > 0)
		{
			if (AMageCharacter* Mage = Cast<AMageCharacter>(OwnerChar))
				Mage->AddOverheatingStack(BurningEnemies.Num());
		}
	}
	
	
}

bool AFlameThrowerActor::GetStartAndDir(FVector& OutStart, FVector& OutDir) const
{
	if (!OwnerChar.IsValid()) return false;

	if (const AMageCharacter* Mage = Cast<AMageCharacter>(OwnerChar.Get()))
	{
		if (UStaticMeshComponent* Wand = Mage->GetWandMesh())
		{
			if (Wand->DoesSocketExist(MuzzleSocketName))
			{
				OutStart = Wand->GetSocketTransform(MuzzleSocketName, RTS_World).GetLocation();
			}
		}
	}

	const FRotator Aim = OwnerChar->GetControlRotation();
	OutDir = Aim.Vector().GetSafeNormal();
	return true;
}

void AFlameThrowerActor::UpdateDamageZoneTransform()
{
	FVector Start, Dir;
	if (!GetStartAndDir(Start, Dir)) return;

	const float HalfLen = Range * 0.5f;
	const float HalfWidth = Range * FMath::Tan(FMath::DegreesToRadians(ConeHalfAngleDeg));
	const float HalfHeight = 100.f;
	
	const FVector Extent(HalfLen, HalfWidth, HalfHeight);
	DamageZone->SetBoxExtent(Extent);

	
	FVector Center = Start + Dir * HalfLen;
	FRotator Rot = Dir.Rotation();
	DamageZone->SetWorldLocation(Center);
	DamageZone->SetWorldRotation(Rot);
}

void AFlameThrowerActor::EndPlay(const EEndPlayReason::Type Reason)
{
	if (HasAuthority() && !DotGrantedTags.IsEmpty() && DamageZone)
	{
		TArray<AActor*> Overlapped;
		DamageZone->GetOverlappingActors(Overlapped);
		for (AActor* A : Overlapped)
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(A))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(DotGrantedTags);
			}
		}
	}
	
	bFiring = false;
	OnRep_Firing();
	Super::EndPlay(Reason);
}
