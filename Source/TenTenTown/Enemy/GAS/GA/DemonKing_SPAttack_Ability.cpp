// Fill out your copyright notice in the Description page of Project Settings.

#include "DemonKing_SPAttack_Ability.h"

UDemonKing_SPAttack_Ability::UDemonKing_SPAttack_Ability()
{
}

bool UDemonKing_SPAttack_Ability::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UDemonKing_SPAttack_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UDemonKing_SPAttack_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDemonKing_SPAttack_Ability::OnMontageFinished(FName NotifyName, float NewDeltaTime)
{
}

void UDemonKing_SPAttack_Ability::ExecuteDamageAndSpawnProjectile()
{
}
