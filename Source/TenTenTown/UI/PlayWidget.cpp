#include "UI/PlayWidget.h"
#include "UI/MVVM/PlayerStatusViewModel.h"
 #include "UI/MVVM/GameStatusViewModel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PCC/PlayPCComponent.h"
#include "Components/ListView.h"
#include "UI/MVVM/PartyManagerViewModel.h"
#include "UI/Widget/QuickSlotBarWidget.h"
#include "Components/Button.h"


void UPlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 1. UListView* PartyListView가 BindWidget으로 바인딩된 상태인지 확인
    if (!PartyListView)
    {
        UE_LOG(LogTemp, Error, TEXT("PartyListView is not bound in UPlayWidget!"));
        return;
    }
	UE_LOG(LogTemp, Log, TEXT("PartyListView is successfully bound in UPlayWidget."));

    if (OnTradeButton)
    {
        OnTradeButton->OnClicked.AddDynamic(this, &UPlayWidget::OnOffButtonClicked);
    }
}

void UPlayWidget::SetPlayerStatusViewModel(UPlayerStatusViewModel* InViewModel)
{
    PlayerStatusViewModel = InViewModel;

    // 이 시점에서 ViewModel이 자식 위젯(예: UPlayerStatusWidget)에도 필요하다면 전달합니다.
}

void UPlayWidget::SetPartyManagerViewModel(UPartyManagerViewModel* ViewModel)
{
	UE_LOG(LogTemp, Log, TEXT("[UPlayWidget] SetPartyManagerViewModel called."));
    // 1. ViewModel 참조 저장
    PartyManagerViewModel = ViewModel;

    // 2. ViewModel이 유효하고 UListView가 바인딩되어 있다면 목록 설정
    if (PartyManagerViewModel && PartyListView)
    {
        // 이 시점에 PartyManagerViewModel은 이미 GameState를 구독하고 초기 목록을 가지고 있습니다.
        // UListView의 SetListItems 함수를 사용하여 목록 데이터를 주입합니다.
        PartyListView->SetListItems(PartyManagerViewModel->GetPartyMembers());

        // 3. 목록이 동적으로 변경될 때 UListView를 갱신하는 옵션 설정 (선택 사항)
        // SetIsRefreshable(true)는 FFieldNotify를 사용하여 목록이 변경될 때 UListView를 자동으로 갱신합니다.
        // UPROPERTY(BlueprintReadOnly, Category = "MVVM") TObjectPtr<UPartyManagerViewModel> PartyManagerViewModel;
        // 이 속성에 FieldNotify가 설정되어 있다면, PartyListView->SetListItems(PartyManagerViewModel->GetPartyMembers())만으로 충분할 수 있습니다.
    }
    else if (!PartyListView)
    {
        UE_LOG(LogTemp, Error, TEXT("[UPlayWidget] SetPartyManagerViewModel: PartyListView is NULL. Check UMG layout."));
    }
}

void UPlayWidget::SetGameStatusViewModel(UGameStatusViewModel* InViewModel)
{
    GameStatusViewModel = InViewModel;
}
void UPlayWidget::SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* InViewModel)
{
    QuickSlotManagerViewModel = InViewModel;
    if (!QuickSlotManagerViewModel) { return; }

    if (QuickSlotBar)
    {
		QuickSlotBar->SetQuickSlotManagerViewModel(QuickSlotManagerViewModel, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[UPlayWidget] SetQuickSlotManagerViewModel: QuickSlotBar is NULL. Check UMG layout."));
	}
    if (QuickSlotBarItem)
    {
        QuickSlotBarItem->SetQuickSlotManagerViewModel(QuickSlotManagerViewModel, true);
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("[UPlayWidget] SetQuickSlotManagerViewModel: QuickSlotBarItem is NULL. Check UMG layout."));
    }
}



void UPlayWidget::HideWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UPlayWidget::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UPlayWidget::SetsPartyListView()
{
    PartyListView->SetListItems(PartyManagerViewModel->GetPartyMembers());
}

void UPlayWidget::OnOffButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("OnOffButtonClicked"));
    if (PlayerStatusViewModel)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnOffButtonClicked viewModels"));
        PlayerStatusViewModel->OnOffTraderWindow(true);
    }
}

