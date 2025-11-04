// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_LevelUP.h"

#include "AbilitySystemComponent.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"

void UGA_LevelUP::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC->GetNumericAttribute(UAS_FighterAttributeSet::GetLevelAttribute())>=10.f) return;
	
	if (GE_LevelUp)
	{
		ASC->ApplyGameplayEffectToSelf(GE_LevelUp->GetDefaultObject<UGameplayEffect>(),1.f,ASC->MakeEffectContext());
	}
	if (GE_AfterLevelUp)
	{
		float Level = ASC->GetNumericAttribute(UAS_FighterAttributeSet::GetLevelAttribute());
		ASC->ApplyGameplayEffectToSelf(GE_AfterLevelUp->GetDefaultObject<UGameplayEffect>(),Level,ASC->MakeEffectContext());
	}
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_LevelUP::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

