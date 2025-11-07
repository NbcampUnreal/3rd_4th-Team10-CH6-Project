#include "FireballProjectile.h"

#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"

AFireballProjectile::AFireballProjectile()
{
	bReplicates = true;
	SetReplicateMovement(true);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(12.f);
	Collision->SetCollisionProfileName(TEXT("Projectile"));
	Collision->OnComponentHit.AddDynamic(this, &AFireballProjectile::OnHit);
	
	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->bRotationFollowsVelocity = true;
	Move->ProjectileGravityScale = 1.0f;
	Move->InitialSpeed = Move->MaxSpeed = 2400.f;
	InitialLifeSpan = 5.f;
	
	PrimaryActorTick.bCanEverTick = false;
}

void AFireballProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Collision)
	{
		if (APawn* Inst = GetInstigator())
		{
			if (AActor* InstActor = Cast<AActor>(Inst))
			{
				Collision->IgnoreActorWhenMoving(InstActor, true);
			}
		}
		
		if (AActor* Own = GetOwner())
		{
			Collision->IgnoreActorWhenMoving(Own, true);
		}
	}
}

void AFireballProjectile::InitVelocity(const FVector& Dir, float Speed)
{
	const FVector V = Dir.GetSafeNormal() * Speed;
	Move->Velocity = V;
	Move->InitialSpeed = Move->MaxSpeed = Speed;
}

void AFireballProjectile::OnHit(UPrimitiveComponent* HitComponent,
                                AActor* OtherActor,
                                UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse,
                                const FHitResult& Hit)
{
	const FVector Loc = Hit.bBlockingHit ? FVector(Hit.ImpactPoint) : GetActorLocation();
	const FRotator Rot = Hit.bBlockingHit ? FVector(Hit.ImpactNormal).Rotation() : GetActorRotation();
	
	if (HasAuthority())
	{
		DoExplode_Server(Loc, Rot);
		Destroy();
	}
	
}

void AFireballProjectile::DoExplode_Server(const FVector& ExplodeLoc, const FRotator& ExplodeRot)
{
	Multicast_Explode(ExplodeLoc, ExplodeRot);
}

void AFireballProjectile::Multicast_Explode_Implementation(const FVector& ExplodeLoc, const FRotator& ExplodeRot)
{
	if (ExplodeVFX)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplodeVFX,
			ExplodeLoc,
			ExplodeRot,
			FVector(1.f),
			true
		);
	}
}
