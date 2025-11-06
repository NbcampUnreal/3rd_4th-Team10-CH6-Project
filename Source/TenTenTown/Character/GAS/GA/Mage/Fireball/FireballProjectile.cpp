#include "FireballProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
	//추후 변경
	Destroy();
}
