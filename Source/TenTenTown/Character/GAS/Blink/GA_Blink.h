#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Blink.generated.h"

UCLASS()
class TENTENTOWN_API UGA_Blink : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Blink();

	virtual const FGameplayTagContainer* GetCooldownTags() const override;
protected:
	UPROPERTY(EditDefaultsOnly, Category="Blink")
	float MaxDist = 800.f;
	UPROPERTY(EditDefaultsOnly, Category="Blink")
	float TraceRadius = 34.f;
	UFUNCTION()
	void OnBlinkDelayFinished();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	UPROPERTY(EditDefaultsOnly, Category="Blink|Cooldown")
	float CooldownSec = 2.f;
	
private:
	//도착지점 계산
	bool bFindBlinkDest(const ACharacter* Character, FVector& OutDest) const;
	
	FGameplayTagContainer BlinkCooldownTags;
};
