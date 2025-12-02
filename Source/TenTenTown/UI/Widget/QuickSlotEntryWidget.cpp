#include "UI/Widget/QuickSlotEntryWidget.h"
#include "Components/Button.h"
#include "UI/MVVM/QuickSlotEntryViewModel.h"

void UQuickSlotEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (QuickSlotButton)
    {
        // 버튼 클릭 시 OnQuickSlotClicked 함수 호출 설정
        QuickSlotButton->OnClicked.AddDynamic(this, &UQuickSlotEntryWidget::OnQuickSlotClicked);
    }
}

void UQuickSlotEntryWidget::SetEntryViewModel(UQuickSlotEntryViewModel* InViewModel)
{
	UE_LOG(LogTemp, Log, TEXT("UQuickSlotEntryWidget::SetEntryViewModel called."));
    // 이미 할당된 뷰모델이 있다면 해제 로직을 수행할 수 있으나, 여기서는 생략
    EntryViewModel = InViewModel;

    // 뷰모델이 변경되었음을 UMG 바인딩 시스템에 알리는 로직이 필요할 수 있습니다.
    // 하지만 UMG MVVM은 UPROPERTY에 할당되면 자동으로 바인식을 업데이트합니다.
    if (EntryViewModel)
    {
        EntryViewModel->BroadcastAllFieldValues();
    }
    
}

void UQuickSlotEntryWidget::OnQuickSlotClicked()
{
    // EntryViewModel이 유효한지 확인하고, 해당 슬롯의 사용(건물 설치 등) 로직을 서버에 요청합니다.
    if (EntryViewModel)
    {
        UE_LOG(LogTemp, Log, TEXT("Slot Clicked: Index %d"), EntryViewModel->GetSlotIndex());
        // EntryViewModel->ServerRequestUseSlot(); 와 같은 함수 호출 로직이 필요합니다.
    }
}


