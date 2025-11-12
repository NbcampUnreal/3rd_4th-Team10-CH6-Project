#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/GAS/GA/Mage/Base/GA_Mage_Base.h"
#include "GA_Mage_ComboAttack.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TENTENTOWN_API UGA_Mage_ComboAttack : public UGA_Mage_Base
{
	GENERATED_BODY()

public:
	UGA_Mage_ComboAttack();
	
	UPROPERTY(EditDefaultsOnly, Category="ComboAttack|AM")
	TArray<TObjectPtr<UAnimMontage>> ComboAttackAM;

	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag OpenTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Open"));
	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag CloseTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Close"));
	UPROPERTY(EditDefaultsOnly, Category="Combo|Tags")
	FGameplayTag HitTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combo.Hit"));
	
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Trace")
	float TraceRadius = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Trace")
	float TraceDistance = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;

	UPROPERTY(EditDefaultsOnly, Category = "ComboAttack|Flow")
	float ComboTimeOut = 1.f;
	
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnOpen(FGameplayEventData Payload);
	UFUNCTION()
	void OnClose(FGameplayEventData Payload);
	UFUNCTION()
	void OnHit(FGameplayEventData Payload);
	
private:
	int32 ComboIdx = 0;
	bool bWindowOpen = false;
	bool bWantsNext = false;
	FTimerHandle ComboResetTimer;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitOpen = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitClose = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitHit = nullptr;

	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageBlendOut();
	UFUNCTION()
void OnMontageInterrupted();
	UFUNCTION()
	void OnMontageCancelled();

	bool bEndedByMontage = false;

	void PlayMontage();
	void BindEvents();
	void TryAdvance();
	void ScheduleReset();
	void ClearReset();

	void DoTraceAndApply();
	

	
	
};
