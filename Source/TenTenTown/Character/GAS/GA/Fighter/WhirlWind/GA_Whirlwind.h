// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Whirlwind.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGA_Whirlwind : public UGameplayAbility
{
	GENERATED_BODY()
	UGA_Whirlwind();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimMontage", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> WhirlWindMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,category="AnimMontage",meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> WhirlWindLoopMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimMontage",meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> DizzyMontage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Duration",meta=(AllowPrivateAccess="true"))
	int32 Duration = 10.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="SpinTime",meta=(AllowPrivateAccess="true"))
	float SpinTime;
	
	UPROPERTY()
	ACharacter* Character;
	
	UPROPERTY()
	bool bAlreadyEnd;
	UPROPERTY()
	float RotateStartTime;
	UPROPERTY()
	float RotateEndTime;
	
	UFUNCTION()
	void OnEnd();
	UFUNCTION()
	void OnFirstMontageEnd();
	UFUNCTION()
	void OnDurationEnd();
	UFUNCTION()
	void OnDizzyEnd();
	UFUNCTION()
	void OnAttack(const FGameplayEventData Payload);
};
