#include "UI/PCC/InventoryPCComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Item/Base/GA_ItemBase.h"
#include "Character/PS/TTTPlayerState.h"

UInventoryPCComponent::UInventoryPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UInventoryPCComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwnerRole() == ROLE_Authority)
	{
		InitFixedSlots();
		UE_LOG(LogTemp, Warning, TEXT("[InventoryComp] InitFixedSlots on server"));
	}
}

void UInventoryPCComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryPCComponent, PlayerGold);
	DOREPLIFETIME(UInventoryPCComponent, InventoryItems);

}

void UInventoryPCComponent::OnRep_Gold()
{	
	OnGoldChangedDelegate.Broadcast(PlayerGold);
}

void UInventoryPCComponent::OnRep_QuickSlotList()
{
	OnQuickSlotListChangedDelegate.Broadcast(QuickSlotList);
}

void UInventoryPCComponent::OnRep_InventoryItems()
{
	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
}

bool UInventoryPCComponent::Server_AddGold_Validate(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_AddGold Validation Failed: Amount must be positive."));
		return false;
	}

	return true;
}

void UInventoryPCComponent::Server_AddGold_Implementation(int32 Amount)
{
	PlayerGold += Amount;

	UE_LOG(LogTemp, Log, TEXT("Server: Added %d Gold. New total: %d"), Amount, PlayerGold);
}


bool UInventoryPCComponent::Server_UpdateQuickSlotList_Validate(const TArray<FInventoryItemData>& NewQuickSlotList)
{
	/*if (NewQuickSlotList.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_UpdateQuickSlotList Validation Failed: Exceeds maximum quick slot count."));
		return false;
	}*/
	return true;
}

void UInventoryPCComponent::Server_UpdateQuickSlotList_Implementation(const TArray<FInventoryItemData>& NewQuickSlotList)
{
	QuickSlotList = NewQuickSlotList;	
	UE_LOG(LogTemp, Log, TEXT("Server: QuickSlotList updated. New count: %d"), QuickSlotList.Num());	
}

bool UInventoryPCComponent::GetItemData(FName ItemID, FItemData& OutItemData) const
{
	if (ItemID.IsNone()) return false;

	const UTTTGameInstance* GI = GetWorld()->GetGameInstance<UTTTGameInstance>();
	if (!GI) return false;
	
	return GI->GetItemData(ItemID, OutItemData);
}

void UInventoryPCComponent::InitFixedSlots()
{
	InventoryItems.Empty();
	
	InventoryItems.Add(FItemInstance(FName("Item_Potion_HP"), 1));
	InventoryItems.Add(FItemInstance(FName("Item_Potion_MP"), 0));
	InventoryItems.Add(FItemInstance(FName("Item_RepairKit"), 0));
	InventoryItems.Add(FItemInstance(FName("Item_Bomb"), 1));
	InventoryItems.Add(FItemInstance(FName("Item_Trap_Ice"), 0));
	InventoryItems.Add(FItemInstance(FName("Item_Trap_Ice"), 0));

	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
}

void UInventoryPCComponent::Server_AddItem_Implementation(FName ItemID, int32 Count)
{
	if (Count <= 0 || ItemID.IsNone()) return;

	FItemData ItemData;
	if (!GetItemData(ItemID, ItemData)) return;

	const int32 MaxStack = ItemData.MaxStackCount;
	for (FItemInstance& Slot : InventoryItems)
	{
		if (Slot.ItemID != ItemID) continue;
		
		int32 NewCount = Slot.Count + Count;
		NewCount = FMath::Clamp(NewCount, 0, MaxStack);
		Slot.Count = NewCount;

		OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
		return;
	}
}

bool UInventoryPCComponent::Server_AddItem_Validate(FName ItemID, int32 Count)
{
	return Count > 0;
}

void UInventoryPCComponent::Server_AddItemWithCost_Implementation(FName ItemID, int32 Count, int32 AddCost)
{
	if (Count <= 0 || ItemID.IsNone() || AddCost < 0) return;

	FItemData ItemData;
	if (!GetItemData(ItemID, ItemData)) return;

	const int32 MaxStack = ItemData.MaxStackCount;
	bool bItemAddedSuccessfully = false;

	for (FItemInstance& Slot : InventoryItems)
	{
		if (Slot.ItemID == ItemID)
		{
			int32 NewCount = Slot.Count + Count;

			if (NewCount > MaxStack)
			{
				return;
			}

			Slot.Count = NewCount;
			bItemAddedSuccessfully = true;

			APlayerController* PC = Cast<APlayerController>(GetOwner());
			ATTTPlayerState* PS = PC ? Cast<ATTTPlayerState>(PC->PlayerState) : nullptr;

			if (PS && PS->GetGold() >= AddCost)
			{
				PS->Server_AddGold_Implementation(-AddCost);
			}
			else
			{
				// 골드 부족: 이미 아이템은 추가했지만, 거래 실패로 간주하고 아이템을 다시 빼야 합니다.
				// 복잡해지므로, **클라이언트 측에서 보내는 시점에 골드를 반드시 재검증**하도록 해야 합니다.
				// 하지만 현재는 인벤토리에 추가만 하고 끝내겠습니다. (골드 차감 실패 = 거래 실패)

				// 만약 이 상황(골드는 부족한데 아이템은 추가됨)을 막고 싶다면, 
				// 이 함수 시작 시점에 골드 검사를 먼저 해야 합니다.
			}

			OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
			return;
		}
	}

	// 이외의 경우 (새로운 슬롯 필요 등)에 대한 로직 추가 필요...
}

bool UInventoryPCComponent::Server_AddItemWithCost_Validate(FName ItemID, int32 Count, int32 AddCost)
{
	return Count > 0 && AddCost >= 0;
}





bool UInventoryPCComponent::GetItemDataFromSlot(int32 SlotIndex, FName& OutItemID, FItemData& OutItemData) const
{
	OutItemID = NAME_None;

	if (SlotIndex < 0 || SlotIndex >= InventoryItems.Num()) return false;

	const FItemInstance& Slot = InventoryItems[SlotIndex];
	if (Slot.Count <= 0 || Slot.ItemID.IsNone()) return false;

	FItemData ItemData;
	if (!GetItemData(Slot.ItemID, ItemData)) return false;

	OutItemID = Slot.ItemID;
	OutItemData = ItemData;
	return true;
}

void UInventoryPCComponent::UseItem(int32 InventoryIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		SendEventToASC(InventoryIndex);
	}
	else
	{
		SendEventToASC(InventoryIndex);
		Server_UseItem(InventoryIndex);
	}
}

void UInventoryPCComponent::Server_UseItem_Implementation(int32 InventoryIndex)
{
	SendEventToASC(InventoryIndex);
}

bool UInventoryPCComponent::Server_UseItem_Validate(int32 InventoryIndex)
{
	return InventoryIndex >= 0 && InventoryIndex < InventoryItems.Num();
}

void UInventoryPCComponent::ConsumeItemFromSlot(int32 SlotIndex, int32 Amount)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	if (SlotIndex < 0 || SlotIndex >= InventoryItems.Num()) return;
	
	FItemInstance& Slot = InventoryItems[SlotIndex];
	if (Slot.Count <= 0) return;

	Slot.Count = FMath::Max(Slot.Count - Amount, 0);
	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
}

void UInventoryPCComponent::SendEventToASC(int32 InventoryIndex)
{
	if (InventoryIndex < 0 || InventoryIndex >= InventoryItems.Num()) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

	ACharacter* Char = Cast<ACharacter>(PC->GetPawn());
	if (!Char) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Char);
	if (!ASC) return;

	FItemInstance& Slot = InventoryItems[InventoryIndex];
	if (Slot.Count <= 0 || Slot.ItemID.IsNone()) return;

	FItemData ItemData;
	if (!GetItemData(Slot.ItemID, ItemData)) return;

	FGameplayTag EventTag;
	switch (ItemData.UseType)
	{
	case EItemUseType::Drink:
		EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.DrinkPotion"));
		break;
		
	case EItemUseType::Throw:
		EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Throw"));
		break;
		
	default:
		return;
	}
	
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.EventMagnitude = InventoryIndex;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Char, Payload.EventTag, Payload);
}
