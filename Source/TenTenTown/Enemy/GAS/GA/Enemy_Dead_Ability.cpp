// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Dead_Ability.h"

#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/System/PoolSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UEnemy_Dead_Ability::UEnemy_Dead_Ability()
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Dead;
	AbilityTriggers.Add(TriggerData);
}

void UEnemy_Dead_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("DeadAbility FINALLY Activated!"));

	AEnemyBase* Actor = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
   this,
   NAME_None,
   Actor->DeadMontage,
   1.0f);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnDeathMontageFinished);
		MontageTask->Activate();
	}
	else
	{
		OnDeathMontageFinished();
	}
}

void UEnemy_Dead_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemy_Dead_Ability::OnDeathMontageFinished()
{
	AEnemyBase* Actor = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());

	if (Actor && Actor->HasAuthority())
	{
		Actor->DropGoldItem(); 

		if (UWorld* World = Actor->GetWorld())
		{
			if (UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
			{
				if (UPoolSubsystem* PoolSubsystem = GI->GetSubsystem<UPoolSubsystem>())
				{
					PoolSubsystem->ReleaseEnemy(Actor);

					if (UAbilitySystemComponent* ASC = Actor->GetAbilitySystemComponent())
					{
						ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Dead);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UDeadTask:Release Failed"));
				}
			}
		
		}
	}
    
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
