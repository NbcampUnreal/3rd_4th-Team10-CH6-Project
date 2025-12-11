#include "UI/Widget/ResultWidget.h"
#include "Components/ListView.h"






void UResultWidget::SetLobbyViewModel(ULobbyViewModel* InViewModel)
{
	LobbyViewModel = InViewModel;
}

void UResultWidget::SetResultListView()
{
	ResultListView->SetListItems(LobbyViewModel->GetResultVMs());
}
