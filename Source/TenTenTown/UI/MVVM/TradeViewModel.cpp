#include "UI/MVVM/TradeViewModel.h"
#include "Item/Data/ItemData.h"
#include "Engine/Texture2D.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Engine/World.h"
#include "GameSystem/GameMode/TTTGameModeBase.h"





#pragma region MainRegion
//void UTradeViewModel::SetPlayerGold(int32 NewGold)
//{
//    PlayerGold = NewGold;
//    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerGold);
//}

//void UTradeViewModel::SetTargetImage(UTexture2D* NewTexture2D)
//{
//    TargetImage = NewTexture2D;
//    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TargetImage);
//}
//
//void UTradeViewModel::SetTargetName(FText& NewName)
//{
//    TargetName = NewName;
//    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TargetName);
//}
//
//void UTradeViewModel::SetTargetDes(FText& NewDes)
//{
//    TargetDes = NewDes;
//    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TargetDes);
//}
//
//void UTradeViewModel::SetTargetPrice(FText& NewPrice)
//{
//    TargetPrice = NewPrice;
//    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TargetPrice);
//}

//void UTradeViewModel::SetHeadItem(FItemData& NewItemData)
//{
//    SetTargetImage(NewItemData.ItemImage.Get());
//    SetTargetName(NewItemData.ItemName);
//    SetTargetDes(NewItemData.Description);
//    FText PriceText = FText::AsNumber(NewItemData.SellPrice);
//    SetTargetPrice(PriceText);
//}
#pragma endregion


#pragma region SlotRegion
UInventoryPCComponent* UTradeViewModel::GetInventoryPCComponent() const
{
    return CachedInventory;
}
void UTradeViewModel::InitializeViewModel(UPlayPCComponent* CachedPlayPCC, ATTTPlayerState* InPlayerState, UTTTGameInstance* TTGI)
{
    if (!InPlayerState) // ASC 유효성 검사 추가
    {
        UE_LOG(LogTemp, Error, TEXT("ManagerVM 초기화 실패: PlayerState 또는 ASC가 유효하지 않습니다."));
        return;
    }


    //인벤토리 찾음
    APlayerController* PC = Cast<APlayerController>(InPlayerState->GetOwner());
    if (!PC) { return; }
    CachedInventory = PC->FindComponentByClass<UInventoryPCComponent>();
    if (!CachedInventory) { return; }

    CachedInventory->OnInventoryItemsChangedDelegate.AddDynamic(this, &UTradeViewModel::OnInventoryUpdated);

    CachedPlayPCComponent = CachedPlayPCC;
    //델리게이트 구독
    //변경된 정보를 받고 처리해야됨..


    CreateTradeSlotEntries(TTGI);
    
    

    //퀵슬롯 초기화
    OnTradeSlotListChanged(CachedInventory->GetQuickSlotList());
}


void UTradeViewModel::CreateTradeSlotEntries(UTTTGameInstance* TTGI)
{
	UE_LOG(LogTemp, Log, TEXT("aaaaCreating Trade Slot Entries..."));
    
    if (!TTGI || !TTGI->ItemDataTable) { return; }

	UE_LOG(LogTemp, Log, TEXT("TTTGameInstance and ItemDataTable are valid in TradeViewModel."));
    UDataTable* ItemTable = TTGI->ItemDataTable;

    // ⭐ 1. RowMap에서 모든 RowName을 추출하여 배열에 저장 (오버헤드 한 번) ⭐
    TArray<FName> AllRowNames;
    ItemTable->GetRowMap().GetKeys(AllRowNames);
    int32 IndexCounts = AllRowNames.Num();

    TradeSlotEntryVMs.Empty();
    TradeSlotEntryVMs.SetNum(IndexCounts);

	UE_LOG(LogTemp, Log, TEXT("Total Item Rows Found: %d"), IndexCounts);
    for (int32 i = 0; i < IndexCounts; ++i)
    {
        // 2. 인덱스 i에 해당하는 RowName을 배열에서 가져옵니다.
        const FName CurrentRowName = AllRowNames[i];

        // 3. RowName을 사용하여 데이터 테이블에서 FItemData를 가져옵니다. (가장 효율적)
        const FItemData* ItemDataPtr = ItemTable->FindRow<FItemData>(CurrentRowName, TEXT("TradeSlotCreation"));

		UE_LOG(LogTemp, Log, TEXT("Processing Item Row %d: %s"), i, *CurrentRowName.ToString());
        if (ItemDataPtr)
        {
            UTradeSlotViewModel* NewSlotVM = NewObject<UTradeSlotViewModel>(this);

            // 4. 슬롯 VM 초기화
            // (InitializeSlot 함수가 FItemData와 FName을 받도록 가정)
            NewSlotVM->SetSlotItem(*ItemDataPtr, CurrentRowName);

            NewSlotVM->SetUpPlayPCC(CachedPlayPCComponent);


            TradeSlotEntryVMs[i] = NewSlotVM;
            
        }
    }

    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TradeSlotEntryVMs);
}

TArray<UTradeSlotViewModel*> UTradeViewModel::GetPartyMembers() const
{
    // 멤버 변수 (TObjectPtr 배열)를 RAW 포인터 배열로 변환하여 반환
    TArray<UTradeSlotViewModel*> RawPtrArray;
    for (const TObjectPtr<UTradeSlotViewModel>& Member : TradeSlotEntryVMs)
    {
        // TObjectPtr에서 RAW 포인터를 얻습니다.
        RawPtrArray.Add(Member.Get());
    }
    return RawPtrArray;
}



//수량 데이터 초기화
void UTradeViewModel::OnTradeSlotListChanged(const TArray<FInventoryItemData>& NewQuickSlotList)
{
    // 리스트 크기 확인 (안정성)
    if (NewQuickSlotList.Num() != TradeSlotEntryVMs.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("QuickSlotManagerViewModel: Received list size mismatch (%d vs %d)."), NewQuickSlotList.Num(), TradeSlotEntryVMs.Num());
    }

    
}

void UTradeViewModel::CallSlotDelegate()
{
    UE_LOG(LogTemp, Log, TEXT("CallSlotDelegate called"));
    //TradeSlotEntryVMs를 순회하면서 BroadcastAllFieldValues()함수 실행
    for (UTradeSlotViewModel* SlotVM : TradeSlotEntryVMs)
    {
        if (SlotVM)
        {
            SlotVM->BroadcastAllFieldValues();
        }
    }
}

void UTradeViewModel::OnInventoryUpdated(const TArray<FItemInstance>& NewItems)
{
    //TradeSlotEntryVMs의 ItemName이랑 같은 NewItems를 찾아서 TradeSlotEntryVMs[i]의 특정 함수 실행
    UE_LOG(LogTemp, Log, TEXT("Inventory Updated. Notifying Slot ViewModels."));
    for (UTradeSlotViewModel* SlotVM : TradeSlotEntryVMs)
    {
        if (IsValid(SlotVM))
        {
            // 1. NewItems 배열 전체를 각 슬롯 ViewModel에 전달합니다.
            //    슬롯 VM은 자신의 ItemID를 기준으로 Count를 계산하고 MVVM 알림을 보냅니다.
            SlotVM->UpdateCurrentCount(NewItems);
        }
    }
}



#pragma endregion

