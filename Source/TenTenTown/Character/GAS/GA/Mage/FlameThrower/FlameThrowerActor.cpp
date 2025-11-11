#include "FlameThrowerActor.h"

#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

AFlameThrowerActor::AFlameThrowerActor()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	VFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	VFX->SetupAttachment(Root);
	VFX->SetAutoActivate(false);
	
	PrimaryActorTick.bCanEverTick = false;
}

void AFlameThrowerActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFlameThrowerActor, Range);
	DOREPLIFETIME(AFlameThrowerActor, ConeHalfAngleDeg);
	DOREPLIFETIME(AFlameThrowerActor, bFiring);  
}

void AFlameThrowerActor::Init(float InInterval, float InDPS, float InConeHalfAngleDeg, float InMaxChannelTime)
{
	if(!HasAuthority()) return;
	
	OwnerChar = Cast<ACharacter>(GetOwner());
	TickInterval = FMath::Max(0.1, InInterval);
	DamagePerTick = InDPS;
	ConeHalfAngleDeg = InConeHalfAngleDeg;

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

void AFlameThrowerActor::EndPlay(const EEndPlayReason::Type Reason)
{
	bFiring = false;
	OnRep_Firing();
	Super::EndPlay(Reason);
}