// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_LevelUP.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_LevelUP : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GE|LEVELUP")
	TSubclassOf<UGameplayEffect> GE_LevelUp;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GE|LEVELUP")
	TSubclassOf<UGameplayEffect> GE_AfterLevelUp;
	
};
