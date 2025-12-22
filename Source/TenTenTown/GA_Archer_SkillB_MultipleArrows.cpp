// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Archer_SkillB_MultipleArrows.h"
#include "Character/Characters/Archer/ArcherCharacter/ArcherCharacter.h"
#include "GameFramework/Character.h"

void UGA_Archer_SkillB_MultipleArrows::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	AvatarCharacter = Cast<AArcherCharacter>(GetAvatarActorFromActorInfo());
	EquippedBow = AvatarCharacter->GetEquippedBow();
	
	
}

void UGA_Archer_SkillB_MultipleArrows::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Archer_SkillB_MultipleArrows::OnEndMontage()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
