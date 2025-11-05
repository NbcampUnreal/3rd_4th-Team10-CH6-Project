#include "Fireball_Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/Character.h"

AFireball_Projectile::AFireball_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;

	InitialSpeed = 3000.f;
	MaxSpeed = 3000.f;
	AlignActorWithVelocity = true;
	GravityScale=0.f;
	CollisionRadius=10.f;
	LifeSpan=3.f;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(CollisionRadius);
	
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	
	CollisionComponent->SetupAttachment(RootComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed=InitialSpeed;
	ProjectileMovementComponent->MaxSpeed=MaxSpeed;
	ProjectileMovementComponent->bRotationFollowsVelocity=AlignActorWithVelocity;
	ProjectileMovementComponent->ProjectileGravityScale=GravityScale;
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->SetupAttachment(CollisionComponent);
}

void AFireball_Projectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnHit);
	ProjectileMovementComponent->OnProjectileStop.AddUniqueDynamic(this,&ThisClass::OnStop);

	GetWorldTimerManager().SetTimer(OnTimeOut,[this]()
		{Destroy();},LifeSpan,false,-1);
	
	if (AActor* ProjectileOwner = GetOwner())
		if (ProjectileOwner)
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(Owner))
				if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(OwnerCharacter->GetPlayerState()))
					ASC=TTTPS->GetAbilitySystemComponent();
	
}

void AFireball_Projectile::Destroyed()
{
	Super::Destroyed();
}

void AFireball_Projectile::FireProjectile(const FVector& Direction, const FVector& End, AActor* IgnoreActor)
{
	
	CollisionComponent->IgnoreActorWhenMoving(IgnoreActor,true);
	
	ProjectileMovementComponent->Velocity=Direction.GetSafeNormal()*InitialSpeed;
	ProjectileMovementComponent->Activate(true);
	
}

void AFireball_Projectile::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();
}

void AFireball_Projectile::OnStop(const FHitResult& HitResult)
{
	Destroy();
}



