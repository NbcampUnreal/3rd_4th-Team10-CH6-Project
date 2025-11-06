#include "Fireball_Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Character/Characters/Fighter/FighterCharacter.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/Character.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"

//TODO: 전반적으로 서버 최적화 (사운드 재생이나 이펙트도 현재 서버에서도 재생됨)는 아직 안 했음

AFireball_Projectile::AFireball_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates=true;
	SetReplicateMovement(true);
	
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
	
	SetRootComponent(CollisionComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed=InitialSpeed;
	ProjectileMovementComponent->MaxSpeed=MaxSpeed;
	ProjectileMovementComponent->bRotationFollowsVelocity=AlignActorWithVelocity;
	ProjectileMovementComponent->ProjectileGravityScale=GravityScale;
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->bAutoActivate=false;
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->SetupAttachment(CollisionComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(RootComponent);
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

	if (ProjectileSound)
	{
		AudioComponent->SetSound(ProjectileSound);
		AudioComponent->Play();
	}
}

void AFireball_Projectile::Destroyed()
{
	if (!ASC) ASC= Cast<AFighterCharacter>(GetOwner())->GetAbilitySystemComponent();
	//TODO: 여기서 데미지 처리 로직이 필요하다. 
	FGameplayCueParameters Params;
	Params.Location = GetActorLocation();
	Params.Normal = GetActorForwardVector();
	Params.SourceObject = this;
	Params.Instigator = Owner;
	Params.EffectCauser = this;
	
	ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Fireball_Explode,Params);
	
	Super::Destroyed();
}

void AFireball_Projectile::FireProjectile(const FVector& Direction, AActor* IgnoreActor)
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



