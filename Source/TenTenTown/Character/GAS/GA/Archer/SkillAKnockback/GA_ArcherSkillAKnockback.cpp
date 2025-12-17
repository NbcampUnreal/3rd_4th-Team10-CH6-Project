// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ArcherSkillAKnockback.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGA_ArcherSkillAKnockback::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
		return;
	}
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	if (AttackAnimMontage)
	{
		UAbilityTask_PlayMontageAndWait* AttackAnimMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,FName("Montage_Attack"),AttackAnimMontage);
		
		AttackAnimMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnEndMontage);
		AttackAnimMontageTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnEndMontage);
		AttackAnimMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnEndMontage);
		
		AttackAnimMontageTask->ReadyForActivation();
	}
	
	
}

void UGA_ArcherSkillAKnockback::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ArcherSkillAKnockback::OnEndMontage()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
