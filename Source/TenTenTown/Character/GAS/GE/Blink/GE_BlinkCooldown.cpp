// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_BlinkCooldown.h"

#include "GameplayTagsManager.h"

UGE_BlinkCooldown::UGE_BlinkCooldown()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Cooldown"));
	DurationMagnitude = FGameplayEffectModifierMagnitude(SBC);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Blink")));
}
