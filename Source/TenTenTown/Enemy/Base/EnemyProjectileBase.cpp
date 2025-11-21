// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Base/EnemyProjectileBase.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnemyBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AEnemyProjectileBase::AEnemyProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent); 
	
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
    
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bIsHomingProjectile = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	Mesh->SetupAttachment(Sphere);
	Sphere->SetupAttachment(RootComponent);

}

void AEnemyProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Sphere->OnComponentHit.AddDynamic(this, &AEnemyProjectileBase::OnHit);
}

void AEnemyProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyProjectileBase::SetProjectileSpeed(float Speed)
{
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
}

float AEnemyProjectileBase::GetProjectileSpeed()
{
	return ProjectileMovement->InitialSpeed;
}

void AEnemyProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner() || OtherActor == this)
	{
		return;
	}
	
	if (OtherActor->IsA<ACharacter>())
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
		
		if (TargetASC && DamageEffect)
		{
			FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
			Context.AddInstigator(GetInstigator(), this);
        
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, EffectLevel, Context);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, -(AttackDamage));
				
				FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        
				HitComp->OnComponentHit.RemoveDynamic(this, &AEnemyProjectileBase::OnHit);
			}

			float TargetHP = TargetASC->GetNumericAttributeBase(UAS_FighterAttributeSet::GetHealthAttribute());

			if (ProjectileEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					ProjectileEffect, 
					Mesh, 
					NAME_None,
					FVector::ZeroVector, 
					FRotator::ZeroRotator, 
					EAttachLocation::KeepRelativeOffset, 
					true
				);
			}
			
			
			GEngine->AddOnScreenDebugMessage(
				-1,                 
				5.0f,               
				FColor::Yellow,     
				FString::Printf(TEXT("Target HP : %f"), TargetHP)
			);
			
		}

		// (선택 사항: 충돌 시 시각 효과(Particle/Sound) 재생)
		Destroy();
	}
    

	
}





