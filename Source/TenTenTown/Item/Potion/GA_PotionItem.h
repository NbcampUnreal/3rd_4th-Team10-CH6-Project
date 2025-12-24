#pragma once

#include "CoreMinimal.h"
#include "Item/Base/GA_ItemBase.h"
#include "GA_PotionItem.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class TENTENTOWN_API UGA_PotionItem : public UGA_ItemBase
{
	GENERATED_BODY()
	
public:
	UGA_PotionItem();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnDrinkEvent(const FGameplayEventData Payload);
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DrinkTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.DrinkPotion"));
};
