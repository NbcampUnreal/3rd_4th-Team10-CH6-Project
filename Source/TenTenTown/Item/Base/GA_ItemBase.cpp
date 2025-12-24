#include "GA_ItemBase.h"

#include "BaseItem.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UI/PCC/InventoryPCComponent.h"

UGA_ItemBase::UGA_ItemBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Character.Item")));
}

bool UGA_ItemBase::InitItem(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !TriggerEventData) return false;
	
	SlotIndex = TriggerEventData->EventMagnitude;
	if (SlotIndex < 0) return false;

	Char = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	PC = Cast<APlayerController>(ActorInfo->PlayerController.Get());
	ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!Char || !PC || !ASC) return false;

	InventoryComp = PC->FindComponentByClass<UInventoryPCComponent>();
	if (!InventoryComp) return false;

	FName OutItemID;
	FItemData OutItemData;
	if (!InventoryComp->GetItemDataFromSlot(SlotIndex, OutItemID, OutItemData)) return false;
	
	ItemID = OutItemID;
	ItemData = OutItemData;
	
	ASC->ForceReplication();
	
	return true;
}

void UGA_ItemBase::ConsumeItemOnServer()
{
	if (!InventoryComp) return;
	if (!Char || !Char->HasAuthority()) return;
	if (SlotIndex < 0) return;
	
	InventoryComp->ConsumeItemFromSlot(SlotIndex, 1);
}
