// Fill out your copyright notice in the Description page of Project Settings.


#include "Archer_Arrow.h"

#include "NiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/Characters/Archer/ArrowAfterHit/ArrowAfterHit.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Elements/Framework/TypedElementSelectionSet.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AArcher_Arrow::AArcher_Arrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates=true;
	SetReplicateMovement(true);
	
	MaxSpeed = 2000.f;
	BasicSpeed = 1000.f;
	GravityScale = 0.5;
	bAutoActivate = false;
	bRotationFollowVelocity = true;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetSphereRadius(5.f);
	CollisionComponent->SetCollisionProfileName(FName("CharacterProjectile"));
	SetRootComponent(CollisionComponent);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ArrowSkeletalMesh");
	SkeletalMeshComponent -> SetupAttachment(RootComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->bAutoActivate=false;
	NiagaraComponent->SetupAttachment(RootComponent);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

void AArcher_Arrow::Multicast_PlayEffects_Implementation()
{
	if (NiagaraComponent)
	{
		NiagaraComponent->Activate(true);

	}
}

void AArcher_Arrow::FireArrow(FVector Direction, float SpeedRatio)
{
	FVector NormalizedDirection;
	
	if (Direction.IsNearlyZero())
	{
		NormalizedDirection = GetActorForwardVector();
	}
	else
	{
		NormalizedDirection = Direction.GetSafeNormal();
	}
	
	SetActorRotation(NormalizedDirection.Rotation());
	
	if (ProjectileMovementComponent)
	{
		SpeedRatio = FMath::Clamp(SpeedRatio,0.3f,1.f);
		Damage = Damage*SpeedRatio;
		
		ProjectileMovementComponent->Velocity = NormalizedDirection* ProjectileMovementComponent->InitialSpeed*SpeedRatio;
		ProjectileMovementComponent->Activate();
	}
}

void AArcher_Arrow::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->MaxSpeed = MaxSpeed;
		ProjectileMovementComponent->InitialSpeed = BasicSpeed;
		ProjectileMovementComponent->ProjectileGravityScale = GravityScale;
		ProjectileMovementComponent->bAutoActivate = bAutoActivate;
		ProjectileMovementComponent->bRotationFollowsVelocity = bRotationFollowVelocity;
		
		ProjectileMovementComponent->UpdateComponentToWorld();
	}
}

void AArcher_Arrow::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Error, TEXT("화살 충돌 발생! 대상: %s"), *OtherActor->GetName());
	
	if (!HasAuthority()) return;
	
	if (AfterArrowHitClass)
	{
		FVector FinalArrowSpawnLocation = Hit.Location;
		FRotator FinalArrowRotation = GetActorRotation();
		FName FinalBoneName = Hit.BoneName;
		
		// 힛 결과에 bone name이 없고 충돌한 액터가 캐릭터 클래스라면
		// 라인 트레이스를 한 번 더 해서 bone name을 구해야 한다.
		if (FinalBoneName == NAME_None && Cast<ACharacter>(OtherActor))
		{
			FVector SecondLineTraceStart = Hit.Location;
			FVector SecondLineTraceEnd = Hit.Location + GetActorForwardVector()*1000.f;
			
			FCollisionQueryParams Params;
			Params.AddIgnoredComponent(Cast<ACharacter>(OtherActor)->GetCapsuleComponent());
			
			FHitResult SecondHitResult;
			bool bSuccess = GetWorld()->LineTraceSingleByChannel(SecondHitResult,SecondLineTraceStart,SecondLineTraceEnd,ECC_Camera,Params);
			
			if (bSuccess)
			{
				FinalArrowSpawnLocation = SecondHitResult.Location+ GetActorForwardVector()*25.f;
				FinalBoneName = SecondHitResult.BoneName;
			}
		}
		else
		{
			FinalArrowSpawnLocation = FinalArrowSpawnLocation + GetActorForwardVector()*25.f;
		}
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this->Owner;
		SpawnParameters.Instigator = this->GetInstigator();
		
		AActor* AfterArrowHitActor = GetWorld()->SpawnActor<AActor>(AfterArrowHitClass,FinalArrowSpawnLocation,FinalArrowRotation,SpawnParameters);
		AfterArrowHitActor->AttachToActor(Hit.GetActor(),FAttachmentTransformRules::KeepWorldTransform,FinalBoneName);
	}
	
	// 데미지 적용
	if (SetByCallerClass)
	{
		FGameplayEffectSpecHandle DamageEffectSpecHandle = ASC->MakeOutgoingSpec(SetByCallerClass,1.f,ASC->MakeEffectContext());
		FGameplayEffectSpec DamageEffectSpec = *DamageEffectSpecHandle.Data.Get();
	
		DamageEffectSpec.SetSetByCallerMagnitude(GASTAG::Data_Damage,Damage);
		
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor());
		
		if (TargetASC)
		{
			ASC->ApplyGameplayEffectSpecToTarget(DamageEffectSpec,TargetASC);
		}
	}
	
	Destroy();
}

// Called when the game starts or when spawned
void AArcher_Arrow::BeginPlay()
{
	Super::BeginPlay();
	
	if (AActor* ProjectileOwner = GetOwner())
		if (ProjectileOwner)
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(ProjectileOwner))
				if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(OwnerCharacter->GetPlayerState()))
				{
					ASC=TTTPS->GetAbilitySystemComponent();
				}
	
	if (GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
        
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
		{
			CollisionComponent->IgnoreComponentWhenMoving(OwnerCharacter->GetCapsuleComponent(), true);
		}
	}
	
	CollisionComponent->OnComponentHit.AddUniqueDynamic(this,&ThisClass::OnHit);
	
}

void AArcher_Arrow::SetSetByCallerClass(TSubclassOf<UGameplayEffect> Class)
{
	SetByCallerClass = Class;
}

void AArcher_Arrow::SetDamage(float NewDamage)
{
	this->Damage = NewDamage;
}

void AArcher_Arrow::SetIgnoreActor(AActor* ActorToIgnore)
{
	if (CollisionComponent&&ActorToIgnore)
	{
		CollisionComponent->IgnoreActorWhenMoving(ActorToIgnore,true);
	}
}

void AArcher_Arrow::SetASC(UAbilitySystemComponent* NewASC)
{
	ASC = NewASC;
}

