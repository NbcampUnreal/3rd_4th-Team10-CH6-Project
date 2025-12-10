

#include "UI/Widget/MapSelectWidget.h"
#include "Components/Button.h"
#include "UI/MVVM/LobbyViewModel.h"

void UMapSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (MapButton0)
    {
        MapButton0->OnClicked.AddDynamic(this, &UMapSelectWidget::OnMapButton0);
    }
    if (MapButton1)
    {
        MapButton1->OnClicked.AddDynamic(this, &UMapSelectWidget::OnMapButton1);
    }
}

void UMapSelectWidget::SetViewModel(ULobbyViewModel* InViewModel)
{
    LobbyViewModel = InViewModel;
}


void UMapSelectWidget::OnMapButton0()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->SelectMap(0);
        UE_LOG(LogTemp, Warning, TEXT("Map 0"));
    }
    UE_LOG(LogTemp, Warning, TEXT("Map 0 End"));
}
void UMapSelectWidget::OnMapButton1()
{
    if (LobbyViewModel)
    {
        LobbyViewModel->SelectMap(1);
		UE_LOG(LogTemp, Warning, TEXT("Map 1"));
    }
    UE_LOG(LogTemp, Warning, TEXT("Map 1 End"));
}
