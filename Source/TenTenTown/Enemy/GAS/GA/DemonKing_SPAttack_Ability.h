// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DemonKing_SPAttack_Ability.generated.h"

class UAnimMontage;
class UGameplayEffect;

UCLASS()
class TENTENTOWN_API UDemonKing_SPAttack_Ability : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDemonKing_SPAttack_Ability();

protected:
	TWeakObjectPtr<AActor> CurrentTargetActor;

	virtual bool CanActivateAbility(
	   const FGameplayAbilitySpecHandle Handle,
	   const FGameplayAbilityActorInfo* ActorInfo,
	   const FGameplayTagContainer* SourceTags = nullptr,
	   const FGameplayTagContainer* TargetTags = nullptr,
	   FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(
	   const FGameplayAbilitySpecHandle Handle,
	   const FGameplayAbilityActorInfo* ActorInfo,
	   const FGameplayAbilityActivationInfo ActivationInfo,
	   const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
	   const FGameplayAbilitySpecHandle Handle,
	   const FGameplayAbilityActorInfo* ActorInfo,
	   const FGameplayAbilityActivationInfo ActivationInfo,
	   bool bReplicateEndAbility,
	   bool bWasCancelled) override;

protected:
	// 델리게이트를 AddDynamic으로 바인딩하기 위해 UFUNCTION을 유지합니다.
	UFUNCTION() 
	void OnMontageFinished(FName NotifyName, float NewDeltaTime);

public:
	// Anim Notify에서 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "SP Attack")
	void ExecuteDamageAndSpawnProjectile();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SP Attack")
	UAnimMontage* SPAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SP Attack")
	TSubclassOf<AActor> SwordWaveProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SP Attack")
	TSubclassOf<UGameplayEffect> DamageEffect;
};