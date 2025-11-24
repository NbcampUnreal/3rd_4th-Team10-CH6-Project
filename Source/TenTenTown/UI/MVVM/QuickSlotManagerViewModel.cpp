
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h" // Entry VM 헤더
#include "Character/PS/TTTPlayerState.h" // ATTTPlayerState 헤더

void UQuickSlotManagerViewModel::Initialize(ATTTPlayerState* InPlayerState)
{
    if (!InPlayerState)
    {
        UE_LOG(LogTemp, Error, TEXT("ManagerVM 초기화 실패: PlayerState가 유효하지 않습니다."));
        return;
    }

    PlayerStateWeakPtr = InPlayerState;

    // 퀵슬롯 개수 (9개) 설정
    const int32 QuickSlotCount = 9;
    QuickSlotEntries.Empty();
    //QuickSlotEntries.SetNum(QuickSlotCount);

    for (int32 Index = 0; Index < QuickSlotCount; ++Index)
    {
        // 1. UQuickSlotEntryViewModel 인스턴스 생성 (Outer는 Manager VM)
        UQuickSlotEntryViewModel* EntryVM = NewObject<UQuickSlotEntryViewModel>(this);

        // 2. EntryVM에 PlayerState와 담당 인덱스(0~8)를 전달하며 초기화
        EntryVM->Initialize(InPlayerState, Index);

        // 3. 배열 끝에 추가 (인덱스는 루프 순서대로 0, 1, 2... 로 들어갑니다.)
        QuickSlotEntries.Add(EntryVM);
    }

    UE_LOG(LogTemp, Log, TEXT("[MVVM] QuickSlotManagerViewModel 초기화 완료. %d개 EntryVM 생성."), QuickSlotCount);
}
