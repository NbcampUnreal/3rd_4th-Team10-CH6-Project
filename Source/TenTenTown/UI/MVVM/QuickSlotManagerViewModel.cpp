
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h" // Entry VM 헤더
#include "Character/PS/TTTPlayerState.h" // ATTTPlayerState 헤더

void UQuickSlotManagerViewModel::InitializeViewModel(ATTTPlayerState* InPlayerState, UAbilitySystemComponent* InASC)
{
    if (!InPlayerState || !InASC) // ASC 유효성 검사 추가
    {
        UE_LOG(LogTemp, Error, TEXT("ManagerVM 초기화 실패: PlayerState 또는 ASC가 유효하지 않습니다."));
        return;
    }

    PlayerStateWeakPtr = InPlayerState;
    // ⭐⭐ ASCWeakPtr 캐싱 추가 (UQuickSlotManagerViewModel.h에 선언 필요) ⭐⭐
    // ASCWeakPtr = InASC; 

    // 퀵슬롯 개수 (9개) 설정
    const int32 QuickSlotCount = 9;
    QuickSlotEntries.Empty();

    for (int32 Index = 0; Index < QuickSlotCount; ++Index)
    {
        // 1. UQuickSlotEntryViewModel 인스턴스 생성 (Outer는 Manager VM)
        UQuickSlotEntryViewModel* EntryVM = NewObject<UQuickSlotEntryViewModel>(this);

        // 2. EntryVM에 PlayerState, ASC, 인덱스를 전달하며 초기화 (EntryVM의 시그니처도 수정해야 할 수 있음)
        // EntryVM->Initialize(InPlayerState, InASC, Index); // EntryVM 시그니처에 따라 수정 필요

        // 일단 현재 시그니처에 맞춰 기존대로 호출합니다. (EntryVM->Initialize 시그니처를 확인하세요.)
        EntryVM->InitializeViewModel(InPlayerState, Index);

        // 3. 배열 끝에 추가
        QuickSlotEntries.Add(EntryVM);
    }

    UE_LOG(LogTemp, Log, TEXT("[MVVM] QuickSlotManagerViewModel 초기화 완료. %d개 EntryVM 생성."), QuickSlotCount);
}
