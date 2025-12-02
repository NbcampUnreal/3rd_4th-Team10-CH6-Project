// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/GA/Enemy_Attack_Ability.h"
#include "Enemy_Counter_Ability.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Counter_Ability : public UEnemy_Attack_Ability
{
	GENERATED_BODY()
	UEnemy_Counter_Ability();
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void PlayAttackMontage() override;
	virtual void OnMontageEnded() override;
};
