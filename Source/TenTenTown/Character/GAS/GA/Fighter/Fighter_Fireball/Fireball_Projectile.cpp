#include "Fireball_Projectile.h"

#include "DrawDebugHelpers.h"
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
#include "Engine/Engine.h"
#include "GameplayEffect.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"

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

void AFireball_Projectile::SetNiagaraScale(float Secs)
{
	float Scale= FMath::Clamp(Secs/3.f,0.1f,1.f);
	NiagaraComponent->SetFloatParameter("Scale Overall",Scale);
}

void AFireball_Projectile::SetSetbyCallerGameplayEffectClass(TSubclassOf<UGameplayEffect> GameplayEffect)
{
	SetByCallerGameplayEffectClass = GameplayEffect;
}

void AFireball_Projectile::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp,Log,TEXT("ChargeSecFromProjectile : %f"),ChargeSecFromAbility);
	
	const FVector3d ActorScale = GetActorScale3D();

	const USceneComponent* Root = GetRootComponent();
	const FVector3d RootRelScale = Root ? Root->GetRelativeScale3D() : FVector3d(1);
	const FVector3d RootWorldScale = Root ? Root->GetComponentTransform().GetScale3D() : FVector3d(1);

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Scale Actor:%g %g %g  Root(Rel):%g %g %g  Root(World):%g %g %g"),
		ActorScale.X, ActorScale.Y, ActorScale.Z,
		RootRelScale.X, RootRelScale.Y, RootRelScale.Z,
		RootWorldScale.X, RootWorldScale.Y, RootWorldScale.Z);
	
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnHit);
	ProjectileMovementComponent->OnProjectileStop.AddUniqueDynamic(this,&ThisClass::OnStop);
	OnDestroyed.AddUniqueDynamic(this,&ThisClass::DestroyBinding);
	
	GetWorldTimerManager().SetTimer(OnTimeOut,[this]()
		{Destroy();},LifeSpan,false,-1);
	
	if (AActor* ProjectileOwner = GetOwner())
		if (ProjectileOwner)
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(ProjectileOwner))
				if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(OwnerCharacter->GetPlayerState()))
					ASC=TTTPS->GetAbilitySystemComponent();

	if (ProjectileSound)
	{
		AudioComponent->SetSound(ProjectileSound);
		AudioComponent->Play();
	}
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

void AFireball_Projectile::DestroyBinding(AActor* DestroyedActor)
{
	if (!ASC)
	{
		ASC= Cast<AFighterCharacter>(GetOwner())->GetAbilitySystemComponent();
		GEngine->AddOnScreenDebugMessage(31232,10.f,FColor::Green,TEXT("no asc from projectile"));
	}

	const FVector3d Center = GetActorLocation();
	const float Radius = 120.f*FMath::Clamp(ChargeSecFromAbility/2,1.f,1.5f);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ProjectileOverlap), false, this);
	TArray<FOverlapResult> Overlaps;

	const bool bAny = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		Center,
		FQuat::Identity,
		ObjParams,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);

	DrawDebugSphere(GetWorld(),Center,Radius,32,FColor::Green,true);
	if (bAny)
	{
		TSet<AActor*> HittedActor;
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SetByCallerGameplayEffectClass,1.f,ASC->MakeEffectContext());
		SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Damage,-200.f);
		
		for (const FOverlapResult& R : Overlaps)
			if (AActor* A = R.GetActor())
			{
				if (!Cast<AEnemyBase>(A)) continue;
				HittedActor.Add(A);
			}

		for (auto* A :HittedActor)
		{
			UAbilitySystemComponent* TargetASC = Cast<AEnemyBase>(A)->GetAbilitySystemComponent();

			const FActiveGameplayEffectHandle H = ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			if (H.WasSuccessfullyApplied())
			{
				GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("GE Applied To TargetASC")));
			}
			
			if (TargetASC)
			{
				const UAS_EnemyAttributeSetBase* AS = TargetASC->GetSet<UAS_EnemyAttributeSetBase>();
				
				if (AS)
				{
					float TargetHealth = AS->GetHealth();
					GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("TargetHealth : %f"),TargetHealth));
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("NoTargetAS")));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Projectile Overlap] None"));
	}
	
	FGameplayCueParameters Params;
	Params.Location = GetActorLocation();
	Params.Normal = GetActorForwardVector();
	Params.SourceObject = this;
	Params.Instigator = Owner;
	Params.EffectCauser = this;
	Params.RawMagnitude =ChargeSecFromAbility;
	
	ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Fireball_Explode,Params);
}

