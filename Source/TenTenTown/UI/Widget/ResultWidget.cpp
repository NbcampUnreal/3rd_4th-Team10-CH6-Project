#include "UI/Widget/ResultWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"




void UResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UResultWidget::OnQuitButtonClicked);
	}
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UResultWidget::OnRestartButtonClicked);
	}
}


void UResultWidget::SetLobbyViewModel(ULobbyViewModel* InViewModel)
{
	LobbyViewModel = InViewModel;
}

void UResultWidget::SetResultListView()
{
	ResultListView->SetListItems(LobbyViewModel->GetResultVMs());
}

void UResultWidget::OnQuitButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Button Clicked"));
}

void UResultWidget::OnRestartButtonClicked()
{
	LobbyViewModel->LobbyResetBt();
}
