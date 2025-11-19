#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Mage_Passive_ManaRegen.generated.h"

UCLASS()
class TENTENTOWN_API UGA_Mage_Passive_ManaRegen : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Mage_Passive_ManaRegen();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ManaRegen")
	TSubclassOf<UGameplayEffect> ManaRegenEffectClass;
};
