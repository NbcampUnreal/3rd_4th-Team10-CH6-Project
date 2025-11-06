#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Mage_Base.generated.h"

UCLASS()
class TENTENTOWN_API UGA_Mage_Base : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Mage_Base();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void EndAbilityClean(bool bCancel);
};
