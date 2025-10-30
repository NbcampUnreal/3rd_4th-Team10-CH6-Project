// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Jump.generated.h"

/**
 * 
 */

class ACharacter;

UCLASS()
class TENTENTOWN_API UGA_Jump : public UGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AM|DoubleJump",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> DoubleJumpMontage;
	
	UFUNCTION()
	void OnLanded(EMovementMode MovementMode);

	UPROPERTY()
	uint8 JumpCount =0;
	UPROPERTY()
	TObjectPtr<ACharacter> Character = nullptr;
};
