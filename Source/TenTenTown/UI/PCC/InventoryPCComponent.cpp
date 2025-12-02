#include "UI/PCC/InventoryPCComponent.h"
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

	//OnRep_Gold 함수를 호출하도록 설정
	DOREPLIFETIME(UInventoryPCComponent, PlayerGold);

	// 필요하다면 컴포넌트가 오너 액터에만 복제되도록 설정하는 것도 고려
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

	// 'PlayerGold'는 ReplicatedUsing = OnRep_Gold로 설정되었으므로,
	// 서버에서 이 값이 변경되면 모든 클라이언트에게 자동으로 값이 복제되고 
	// 클라이언트에서는 OnRep_Gold() 함수가 호출됩니다.

	UE_LOG(LogTemp, Log, TEXT("Server: Added %d Gold. New total: %d"), Amount, PlayerGold);

	// 서버 자체에서는 OnRep_Gold가 호출되지 않으므로, 델리게이트를 직접 호출해야 할 수도 있습니다.
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
	// 'QuickSlotList'는 ReplicatedUsing = OnRep_QuickSlotList로 설정되었으므로,
	// 서버에서 이 값이 변경되면 모든 클라이언트에게 자동으로 값이 복제되고 
	// 클라이언트에서는 OnRep_QuickSlotList() 함수가 호출됩니다.
	UE_LOG(LogTemp, Log, TEXT("Server: QuickSlotList updated. New count: %d"), QuickSlotList.Num());
	// 서버 자체에서는 OnRep_QuickSlotList가 호출되지 않으므로, 델리게이트를 직접 호출해야 할 수도 있습니다.
}