#include "GA_Mage_Base.h"

UGA_Mage_Base::UGA_Mage_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Mage_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	EndAbilityClean(false);
}

void UGA_Mage_Base::EndAbilityClean(bool bCancel)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bCancel, false);
}