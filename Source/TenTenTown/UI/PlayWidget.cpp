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
#include "UI/MVVM/SkillCoolTimeViewModel.h"
#include "UI/Widget/SkillCoolTimeWidget.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"


void UPlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //UListView* PartyListView가 BindWidget으로 바인딩된 상태인지 확인
    if (!PartyListView)
    {
        return;
    }

    if (OnTradeButton)
    {
        OnTradeButton->OnClicked.AddDynamic(this, &UPlayWidget::OnOffButtonClicked);
    }
}

void UPlayWidget::SetPlayerStatusViewModel(UPlayerStatusViewModel* InViewModel)
{
    PlayerStatusViewModel = InViewModel;
}

void UPlayWidget::SetPartyManagerViewModel(UPartyManagerViewModel* ViewModel)
{
    PartyManagerViewModel = ViewModel;

    if (PartyManagerViewModel && PartyListView)
    {
        PartyListView->SetListItems(PartyManagerViewModel->GetPartyMembers());
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
void UPlayWidget::SetSkillCoolTimeViewModel(USkillCoolTimeViewModel* InViewModel)
{
    SkillCoolTimeViewModel = InViewModel;
    
	SetSkillCoolTimeListView();
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

void UPlayWidget::SetSkillCoolTimeListView()
{
    if (SkillCoolTimeViewModel)
    {
        SkillListView->SetListItems(SkillCoolTimeViewModel->GetSlotVMs());
    }
}

void UPlayWidget::OnOffButtonClicked()
{
    if (PlayerStatusViewModel)
    {
        PlayerStatusViewModel->OnOffTraderWindow(true);
    }
}



