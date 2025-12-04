#include "UI/PCC/InventoryPCComponent.h"

#include "AbilitySystemGlobals.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/PlayerController.h"

UInventoryPCComponent::UInventoryPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UInventoryPCComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryPCComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//OnRep_Gold �Լ��� ȣ���ϵ��� ����
	DOREPLIFETIME(UInventoryPCComponent, PlayerGold);
	DOREPLIFETIME(UInventoryPCComponent, InventoryItems);

	// �ʿ��ϴٸ� ������Ʈ�� ���� ���Ϳ��� �����ǵ��� �����ϴ� �͵� ����
	// DOREPLIFETIME_CONDITION(UInventoryPCComponent, PlayerGold, COND_OwnerOnly);
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

	// 'PlayerGold'�� ReplicatedUsing = OnRep_Gold�� �����Ǿ����Ƿ�,
	// �������� �� ���� ����Ǹ� ��� Ŭ���̾�Ʈ���� �ڵ����� ���� �����ǰ� 
	// Ŭ���̾�Ʈ������ OnRep_Gold() �Լ��� ȣ��˴ϴ�.

	UE_LOG(LogTemp, Log, TEXT("Server: Added %d Gold. New total: %d"), Amount, PlayerGold);

	// ���� ��ü������ OnRep_Gold�� ȣ����� �����Ƿ�, ��������Ʈ�� ���� ȣ���ؾ� �� ���� �ֽ��ϴ�.
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
	// 'QuickSlotList'�� ReplicatedUsing = OnRep_QuickSlotList�� �����Ǿ����Ƿ�,
	// �������� �� ���� ����Ǹ� ��� Ŭ���̾�Ʈ���� �ڵ����� ���� �����ǰ� 
	// Ŭ���̾�Ʈ������ OnRep_QuickSlotList() �Լ��� ȣ��˴ϴ�.
	UE_LOG(LogTemp, Log, TEXT("Server: QuickSlotList updated. New count: %d"), QuickSlotList.Num());
	// ���� ��ü������ OnRep_QuickSlotList�� ȣ����� �����Ƿ�, ��������Ʈ�� ���� ȣ���ؾ� �� ���� �ֽ��ϴ�.
}

bool UInventoryPCComponent::GetItemData(FName ItemID, FItemData& OutItemData) const
{
	if (!ItemDataTable) return false;
	if (ItemID.IsNone()) return false;

	const FItemData* Row = ItemDataTable->FindRow<FItemData>(ItemID, TEXT("GetItemData"));
	if (!Row) return false;

	OutItemData = *Row;
	return true;
}

void UInventoryPCComponent::Server_AddItem_Implementation(FName ItemID, int32 Count)
{
	/*
	if (Count <= 0) return;

	FItemData ItemData;
	if (!GetItemData(ItemID, ItemData)) return;

	int32 AddCount = Count;
	for (FItemInstance& Slot : InventoryItems)
	{
		if (Slot.ItemID != ItemID) continue;

		const int32 MaxCount = ItemData.MaxStackCount;
		const int32 Space = MaxCount - Slot.Count;
		if (Space <= 0) continue;

		const int32 AddNow = FMath::Min(Space, AddCount);
		Slot.Count += AddNow;
		AddCount -= AddNow;

		if (AddCount <= 0) break;
	}

	while (AddCount > 0)
	{
		const int32 MaxCount = ItemData.MaxStackCount;
		const int32 AddNow = FMath::Min(MaxCount, AddCount);

		FItemInstance NewSlot(ItemID, AddCount);
		InventoryItems.Add(NewSlot);

		AddCount -= AddNow;
	}
	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
	*/
}


bool UInventoryPCComponent::Server_AddItem_Validate(FName ItemID, int32 Count)
{
	return Count > 0;
}

void UInventoryPCComponent::Server_UseItem_Implementation(int32 InventoryIndex)
{
	if (InventoryIndex < 0 || InventoryIndex >= InventoryItems.Num()) return;

	FItemInstance& Slot = InventoryItems[InventoryIndex];
	if (Slot.Count <= 0 || Slot.ItemID.IsNone()) return;

	FItemData ItemData;
	if (!GetItemData(Slot.ItemID, ItemData)) return;

	ABaseCharacter* Char = Cast<ABaseCharacter>(GetOwner());
	if (!Char) return;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Char);
	if (!ASC) return;

	if (ItemData.PassiveEffect)
	{
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(this);
		
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(ItemData.PassiveEffect, 1.f, Ctx);
		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(ItemData.ItemTag, ItemData.Magnitude);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}

	Slot.Count--;
	if (Slot.Count <= 0) InventoryItems.RemoveAt(InventoryIndex);
	OnInventoryItemsChangedDelegate.Broadcast(InventoryItems);
}

bool UInventoryPCComponent::Server_UseItem_Validate(int32 InventoryIndex)
{
	return InventoryIndex >= 0;
}