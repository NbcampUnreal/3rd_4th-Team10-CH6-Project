#include "UI/Widget/QuickSlotBarWidget.h"
#include "UI/Widget/QuickSlotEntryWidget.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h"

void UQuickSlotBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 1. 9개의 개별 위젯을 관리 배열에 담습니다.
    QuickSlotEntryWidgets.Empty(9);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_0);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_1);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_2);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_3);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_4);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_5);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_6);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_7);
    QuickSlotEntryWidgets.Add(QuickSlotEntry_8);

    
    // UMG 바인딩 실패로 nullptr이 들어갔다면 제거합니다.
    QuickSlotEntryWidgets.RemoveAll([](const TObjectPtr<UQuickSlotEntryWidget>& Widget) {
        return Widget == nullptr;
        });

    // NativeConstruct 시점에 이미 ManagerVM이 설정되어 있다면 바로 바인딩을 시도합니다.
    BindEntryViewModels();
}

void UQuickSlotBarWidget::SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* ManagerVM)
{
	UE_LOG(LogTemp, Log, TEXT("UQuickSlotBarWidget::SetQuickSlotManagerViewModel called."));
    if (!ManagerVM) return;

    QuickSlotManagerViewModel = ManagerVM;

    // ViewModel이 설정되면 바로 바인딩을 시작합니다.
    BindEntryViewModels();
}


//매니저 VM으로부터 엔트리 VM 목록을 받아 위젯에 설정합니다.
 
void UQuickSlotBarWidget::BindEntryViewModels()
{
	UE_LOG(LogTemp, Log, TEXT("UQuickSlotBarWidget::BindEntryViewModels called."));
    // ManagerVM이 없거나 위젯들이 아직 준비되지 않았다면 종료
    if (!QuickSlotManagerViewModel || QuickSlotEntryWidgets.IsEmpty()) return;

	UE_LOG(LogTemp, Log, TEXT("Binding QuickSlot Entry ViewModels to Widgets."));
    // 1. 매니저 VM으로부터 엔트리 VM 리스트를 가져옵니다. (매니저 VM이 생성/관리한 목록)
    const TArray<TObjectPtr<UQuickSlotEntryViewModel>>& EntryViewModels = QuickSlotManagerViewModel->GetQuickSlotEntryVMs();

    // 위젯 개수와 VM 개수 중 작은 것을 기준으로 반복합니다.
    int32 NumToBind = FMath::Min(QuickSlotEntryWidgets.Num(), EntryViewModels.Num());

    for (int32 i = 0; i < NumToBind; ++i)
    {
        UQuickSlotEntryWidget* EntryWidget = QuickSlotEntryWidgets[i].Get();
        UQuickSlotEntryViewModel* EntryVM = EntryViewModels[i].Get();

        if (EntryWidget && EntryVM)
        {
            // 2. 뷰모델을 위젯에 설정합니다. (MVVM 연결 완료)
            EntryWidget->SetEntryViewModel(EntryVM);
			EntryVM->SetSlotNumber(i+1); // 슬롯 번호 설정
        }
    }
}
