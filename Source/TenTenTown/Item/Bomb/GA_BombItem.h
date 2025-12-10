#pragma once

#include "CoreMinimal.h"
#include "Item/Base/GA_ItemBase.h"
#include "GA_BombItem.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class TENTENTOWN_API UGA_BombItem : public UGA_ItemBase
{
	GENERATED_BODY()
	
public:
	UGA_BombItem();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnThrowEvent(const FGameplayEventData Payload);
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ThrowTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Throw"));
};
