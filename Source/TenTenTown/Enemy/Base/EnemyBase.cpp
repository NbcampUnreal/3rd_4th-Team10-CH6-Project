// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Enemy/Base/EnemyBase.h"
#include "GameFramework/Character.h"
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
#include "Character/Characters/Base/BaseCharacter.h"
#include "Components/SplineComponent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Components/SplineComponent.h"
#include "Enemy/Data/EnemyData.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Enemy/Route/SplineActor.h"
#include "Enemy/TestEnemy/TestGold.h"
#include "Net/UnrealNetwork.h"
#include "Structure/Crossbow/CrossbowStructure.h"


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
	StateTree->SetAutoActivate(false);

	DetectComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectComponent"));
	if (RootComponent && DetectComponent)
	{
		DetectComponent->SetupAttachment(RootComponent);
	}

	GetMesh()->SetIsReplicated(true);
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = AAIController::StaticClass();


}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, MovedDistance);
	DOREPLIFETIME(AEnemyBase, DistanceOffset);
	DOREPLIFETIME(AEnemyBase, SplineActor);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	//StartTree();
}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		DetectComponent->SetSphereRadius(ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackRangeAttribute()));
		
		AddDefaultAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC is null"));
	}
}

void AEnemyBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Super::Tick(DeltaSeconds);

	LogTimer += DeltaSeconds;

	if (LogTimer >= 3.0f)
	{
		LogAttributeAndTags();
        
		LogTimer = 0.0f;
	}
}

void AEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	DetectComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnDetection);
	DetectComponent->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::EndDetection);
}


void AEnemyBase::OnDetection(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this
		&& (OtherActor->IsA<ABaseCharacter>()
			|| OtherActor->IsA<ACrossbowStructure>()
		))
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

const UAS_EnemyAttributeSetBase* AEnemyBase::GetAttributeSet() const
{
	if (!ASC) return nullptr;
	return DefaultAttributeSet ;
}

void AEnemyBase::StartTree()
{
	if (StateTree)
	{
		StateTree->StartLogic();
	}
}

void AEnemyBase::LogAttributeAndTags()
{

	float CurrentHealth = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetHealthAttribute());
	float MaxHealth = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute());
	float Attack = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute());

	UE_LOG(LogTemp, Display, TEXT("===== Enemy Log: %s ====="), *GetName());
	UE_LOG(LogTemp, Display, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);
	UE_LOG(LogTemp, Display, TEXT("Attack: %.1f"), Attack);
	
	FGameplayTagContainer OwnedTags;
	ASC->GetOwnedGameplayTags(OwnedTags);

	FString TagsString;
	for (const FGameplayTag& Tag : OwnedTags)
	{
		TagsString.Appendf(TEXT("%s, "), *Tag.GetTagName().ToString());
	}

	if (TagsString.IsEmpty())
	{
		TagsString = TEXT("NONE");
	}
	else
	{
		TagsString.RemoveAt(TagsString.Len() - 2); 
	}

	UE_LOG(LogTemp, Display, TEXT("Current Tags: %s"), *TagsString);
	UE_LOG(LogTemp, Display, TEXT("=========================="));
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

	if (!MontageToPlay || !GetMesh())
	{
		return 0.0f;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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
	if (this->HasAuthority())
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

		for (int32 i = 0; i < GoldAmount; ++i)
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

void AEnemyBase::ApplySplineMovementCorrection()
{
	if (!SplineActor || !SplineActor->SplineActor) return;

	USplineComponent* SplineComp = SplineActor->SplineActor;

	float ClampedDistance = FMath::Clamp(MovedDistance, 0.f, SplineComp->GetSplineLength());

	FVector SplineLocation = SplineComp->GetLocationAtDistanceAlongSpline(
		ClampedDistance, ESplineCoordinateSpace::World);

	FVector Direction = SplineComp->GetDirectionAtDistanceAlongSpline(
		ClampedDistance, ESplineCoordinateSpace::World);

	Direction.Normalize();
	const FVector RightVector = FVector::CrossProduct(Direction, FVector::UpVector);

	FVector OffsetVector = RightVector * DistanceOffset;

	FVector NewLocation = SplineLocation + OffsetVector;
	FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AEnemyBase::OnRep_MovedDistance()
{
	ApplySplineMovementCorrection();
}

void AEnemyBase::OnRep_DistanceOffset()
{
	ApplySplineMovementCorrection();
}


void AEnemyBase::Multicast_PlayMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		PlayMontage(MontageToPlay, FMontageEnded(), InPlayRate);
	}
}
