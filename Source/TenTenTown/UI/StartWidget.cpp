#include "UI/StartWidget.h"
#include "Components/Button.h"

void UStartWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UStartWidget::OnMyButtonClicked);
    }
}

void UStartWidget::OnMyButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Button Clicked!"));
}
