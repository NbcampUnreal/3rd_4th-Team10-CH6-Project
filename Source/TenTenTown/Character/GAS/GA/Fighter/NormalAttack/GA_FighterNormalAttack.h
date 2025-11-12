// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_FighterNormalAttack.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_FighterNormalAttack : public UGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> FirstSecondComboMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Anims",meta=(AllowPrivateAccess=true))
	TObjectPtr<UAnimMontage> LastComboMontage;
	
	UPROPERTY()
	bool bIsComboSectionStart;

	UPROPERTY()
	bool bIsComboInputPressed;
	
	UPROPERTY()
	int32 CurrentComboCount;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY()
	TArray<FName> SectionNames;
	UFUNCTION()
	void ComboStart(const FGameplayEventData Data);
	UFUNCTION()
	void ComboEnd(const FGameplayEventData Data);
	UFUNCTION()
	void OnFirstSecondMontageEnd();
	UFUNCTION()
	void OnLastMontageEnd();
	UFUNCTION()
	void OnInterrupted();
};
