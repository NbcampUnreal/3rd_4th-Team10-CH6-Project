#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/BaseGA/BaseGameplayAbility.h"
#include "Item/Data/ItemData.h"
#include "GA_ItemBase.generated.h"


class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class UInventoryPCComponent;
class ABaseCharacter;

UCLASS()
class TENTENTOWN_API UGA_ItemBase : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ItemBase();
	
protected:
	bool InitItem(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* TriggerEventData);
	void ConsumeItemOnServer();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FName ItemID;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FItemData ItemData;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<ABaseCharacter> Char = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<APlayerController> PC = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> ASC = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UInventoryPCComponent> InventoryComp = nullptr;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask = nullptr;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask = nullptr;
};
