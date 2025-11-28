#include "UI/Widget/QuickSlotBarWidget.h"
#include "UI/Widget/QuickSlotEntryWidget.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"

void UQuickSlotBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // InitializeWidget은 PC 컴포넌트에서 호출되므로, 여기서는 별다른 초기화 로직이 필요 없습니다.
}

TArray<UQuickSlotEntryWidget*> UQuickSlotBarWidget::GetEntryWidgets() const
{
    // UMG에서 바인딩된 9개 위젯을 배열로 리턴하는 헬퍼 함수 (반복 작업 최소화)
    return {
        QuickSlotEntry_0, QuickSlotEntry_1, QuickSlotEntry_2,
        QuickSlotEntry_3, QuickSlotEntry_4, QuickSlotEntry_5,
        QuickSlotEntry_6, QuickSlotEntry_7, QuickSlotEntry_8
    };
}

void UQuickSlotBarWidget::InitializeWidget(UQuickSlotManagerViewModel* InManagerVM)
{
    if (!InManagerVM)
    {
        UE_LOG(LogTemp, Error, TEXT("QuickSlotBarWidget 초기화 실패: Manager ViewModel이 유효하지 않습니다."));
        return;
    }

    ManagerViewModel = InManagerVM;

    // 1. UMG에 미리 배치된 자식 위젯 목록을 가져옵니다.
    TArray<UQuickSlotEntryWidget*> EntryWidgets = GetEntryWidgets();

    // 2. Manager VM의 Entry ViewModel 배열을 순회하며 자식 위젯에 주입합니다.
    for (int32 Index = 0; Index < ManagerViewModel->QuickSlotEntries.Num() && Index < EntryWidgets.Num(); ++Index)
    {
        if (EntryWidgets[Index] && ManagerViewModel->QuickSlotEntries.IsValidIndex(Index))
        {
            EntryWidgets[Index]->SetEntryViewModel(ManagerViewModel->QuickSlotEntries[Index]);
            UE_LOG(LogTemp, Log, TEXT("이것은 문제의 코드입니다."));
        }
    }
}

void UQuickSlotBarWidget::SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* ManagerVM)
{
    // 1. Manager ViewModel 캐싱
    ManagerViewModel = ManagerVM;

    if (!ManagerViewModel)
    {
        UE_LOG(LogTemp, Warning, TEXT("Manager ViewModel is null in QuickSlotBarWidget."));
        return;
    }

    // 2. 9개의 Entry ViewModel 목록 가져오기
    const TArray<TObjectPtr<UQuickSlotEntryViewModel>>& EntryVMs = ManagerViewModel->QuickSlotEntries;

    // 3. 자식 위젯 목록 가져오기 (GetEntryWidgets 헬퍼 함수 사용)
    TArray<UQuickSlotEntryWidget*> EntryWidgets = GetEntryWidgets();

    // 4. Entry ViewModel을 개별 Entry Widget에 할당
    for (int32 i = 0; i < EntryWidgets.Num() && i < EntryVMs.Num(); ++i)
    {
        if (EntryWidgets[i] && EntryVMs[i])
        {
            // QuickSlotEntryWidget에 Entry ViewModel을 할당하는 Setter 함수를 호출합니다.
            // (QuickSlotEntryWidget.h에 SetEntryViewModel 함수가 선언되어 있어야 합니다.)
            EntryWidgets[i]->SetEntryViewModel(EntryVMs[i]);
        }
    }
}
