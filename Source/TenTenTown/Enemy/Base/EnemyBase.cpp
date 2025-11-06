// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Enemy/Base/EnemyBase.h"
#include "StateTreeModule.h"
#include "GameplayStateTreeModule/Public/Components/StateTreeComponent.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	if (ASC)
	{
		ASC->SetIsReplicated(true);
		ASC->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	}

	StateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	if (Capsule && SkeletalMesh)
	{
		SkeletalMesh->SetupAttachment(Capsule);
	}
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
}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return ASC;
}


void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::AddDefaultAbility()
{
	for (TSubclassOf<UGameplayAbility> Ability : DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Ability, 1, INDEX_NONE, this);
		ASC->GiveAbility(AbilitySpec);
	}
}

