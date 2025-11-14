// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Enemy/Base/EnemyBase.h"
#include "StateTreeModule.h"
#include "GameplayStateTreeModule/Public/Components/StateTreeComponent.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TTTGamePlayTags.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimInstance.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Enemy/TestEnemy/TestGold.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetNetUpdateFrequency(30.f);
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	if (ASC)
	{
		ASC->SetIsReplicated(true);
		ASC->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	}
	DefaultAttributeSet = CreateDefaultSubobject<UAS_EnemyAttributeSetBase>(TEXT("AttributeSet"));

	StateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetIsReplicated(true);
	if (Capsule && SkeletalMesh)
	{
		SkeletalMesh->SetupAttachment(Capsule);
	}

	DetectComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectComponent"));
	DetectComponent->SetSphereRadius(500.f);
	if (Capsule && DetectComponent)
	{
		DetectComponent->SetupAttachment(Capsule);
	}

	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = AAIController::StaticClass();

}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	
}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		AddDefaultAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC is null"));
	}
}

void AEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//DetectComponent->SetSphereRadius(ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackRangeAttribute()));
	DetectComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnDetection);
	DetectComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::EndDetection);
}


void AEnemyBase::OnDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA<APawn>())
	{
		if (!OverlappedPawns.Contains(OtherActor))
		{
			OverlappedPawns.Add(OtherActor);
			
			SetCombatTagStatus(true);
		}
	}
}

void AEnemyBase::EndDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA<APawn>())
	{
		OverlappedPawns.Remove(OtherActor);

		if (OverlappedPawns.Num() == 0)
		{
			SetCombatTagStatus(false);
		}
	}
}

void AEnemyBase::SetCombatTagStatus(bool IsCombat)
{
	if (ASC)
	{
		FGameplayTag CombatTag = GASTAG::Enemy_State_Combat; 

		if (IsCombat)
		{
			if (!ASC->HasMatchingGameplayTag(CombatTag))
			{
				ASC->AddLooseGameplayTag(CombatTag);
			}
		}
		else
		{
			if (ASC->HasMatchingGameplayTag(CombatTag))
			{
				ASC->RemoveLooseGameplayTag(CombatTag);
			}
		}
	}
}




UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return ASC;
}


void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAS_EnemyAttributeSetBase* AEnemyBase::GetAttributeSet() const
{
	if (!ASC) return nullptr;
	return const_cast<UAS_EnemyAttributeSetBase*>(ASC->GetSet<UAS_EnemyAttributeSetBase>());
}

void AEnemyBase::AddDefaultAbility()
{
	for (TSubclassOf<UGameplayAbility> Ability : DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Ability, 1, INDEX_NONE, this);
		ASC->GiveAbility(AbilitySpec);
	}
}


float AEnemyBase::PlayMontage(UAnimMontage* MontageToPlay, FMontageEnded Delegate, float InPlayRate)
{

	if (!MontageToPlay || !SkeletalMesh)
	{
		return 0.0f;
	}

	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return 0.0f;
	}

	const float Duration = AnimInstance->Montage_Play(MontageToPlay, InPlayRate);
	if (Duration <= 0.f)
	{
		return 0.f;
	}

	FOnMontageEnded MontageEndedDelegate;
	if (Delegate.IsBound())
	{
		MontageEndedDelegate.BindUFunction(Delegate.GetUObject(), Delegate.GetFunctionName());
	}
	
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
	
	return Duration;
}

void AEnemyBase::DropGoldItem()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		const float GoldAmount = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetGoldAttribute());
		const FVector GoldLocation = GetActorLocation();

		const float MinImpulse = 100.0f;              
		const float MaxImpulse = 200.0f;

		if (!GoldItem) return;
		
		ATestGold* GoldItemCDO = Cast<ATestGold>(GoldItem->GetDefaultObject());

		if (!GoldItemCDO) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Template = GoldItemCDO;
		
		// if (GoldAmount <= 0.0f || !GoldItem)
		// {
		// 	return;	
		// }

		for (int32 i = 0; i < 10; ++i)
		{
			FVector SpawnLocation = GoldLocation + FVector(0, 0, 20.f);
			
			ATestGold* SpawnedGold = GetWorld()->SpawnActor<ATestGold>(GoldItem, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedGold)
			{
				if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(SpawnedGold->GetRootComponent()))
				{
					PrimitiveComp->SetSimulatePhysics(true);
					
					FVector RandomDirection = FMath::VRand(); 
					RandomDirection.Z = FMath::Max(0.2f, RandomDirection.Z);
					RandomDirection.Normalize();

					float RandomImpulse = FMath::RandRange(MinImpulse, MaxImpulse);
                
					PrimitiveComp->AddImpulse(RandomDirection * RandomImpulse * PrimitiveComp->GetMass(), NAME_None, true);
				}
			}
		}
	}
}


void AEnemyBase::Multicast_PlayMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		PlayMontage(MontageToPlay, FMontageEnded(), InPlayRate);
	}
}
