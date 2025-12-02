
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "GameFramework/PlayerController.h"

void UQuickSlotManagerViewModel::InitializeViewModel(ATTTPlayerState* InPlayerState)
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

    //델리게이트 구독

    //엔트리 생성
    CreateQuickSlotEntries();

    //퀵슬롯 초기화
    OnQuickSlotListChanged(CachedInventory->GetQuickSlotList());
}


void UQuickSlotManagerViewModel::CreateQuickSlotEntries()
{
    QuickSlotEntryVMs.Empty();
    QuickSlotEntryVMs.SetNum(8); // 퀵슬롯 개수 8개로 고정

    for (int32 i = 0; i < 8; ++i)
    {
        // Entry ViewModel 인스턴스 생성
        UQuickSlotEntryViewModel* NewEntryVM = NewObject<UQuickSlotEntryViewModel>(this);
        NewEntryVM->SlotIndex = i; // 슬롯 인덱스 설정  ...  이거 필요한ㄱ?

        QuickSlotEntryVMs[i] = NewEntryVM;
    }
    //SetSlotNumber

    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(QuickSlotEntryVMs);
}


void UQuickSlotManagerViewModel::OnQuickSlotListChanged(const TArray<FInventoryItemData>& NewQuickSlotList)
{
    // 리스트 크기 확인 (안정성)
    if (NewQuickSlotList.Num() != QuickSlotEntryVMs.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("QuickSlotManagerViewModel: Received list size mismatch (%d vs %d)."), NewQuickSlotList.Num(), QuickSlotEntryVMs.Num());
    }

    // 4. 데이터 분배 (가장 중요한 부분)
    for (int32 Index = 0; Index < QuickSlotEntryVMs.Num(); ++Index)
    {
        UQuickSlotEntryViewModel* EntryVM = QuickSlotEntryVMs[Index];
        if (!EntryVM)
        {
			continue;
        }
        
        if (Index < NewQuickSlotList.Num() && !NewQuickSlotList[Index].ItemName.IsEmpty())//이름이 아니라 초기레벨로 해야할지?
        {
            //정상 배치 동작 수행
            EntryVM->UpdateItemData(NewQuickSlotList[Index]);
        }
        else
        {
            //투명화 수행
            EntryVM->ClearItemData();
        }
    }
}

