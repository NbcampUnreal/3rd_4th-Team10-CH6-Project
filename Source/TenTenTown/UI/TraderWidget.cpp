#include "UI/TraderWidget.h"
#include "Components/Button.h"

void UTraderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BuyButton)
	{
		BuyButton->OnClicked.AddDynamic(this, &UTraderWidget::OnBuyButtonClicked);
	}
	if (ScrollFront)
	{
		ScrollFront->OnClicked.AddDynamic(this, &UTraderWidget::OnScrollFrontClicked);
	}
	if (ScrollBack)
	{
		ScrollBack->OnClicked.AddDynamic(this, &UTraderWidget::OnScrollBackClicked);
	}
}

void UTraderWidget::SetItemNameText(FName ItemName)
{
}

void UTraderWidget::SetItemDesText(FName ItemDes)
{
}

void UTraderWidget::SetItemPriceText(int32 ItemPrice)
{

}

void UTraderWidget::OnBuyButtonClicked()
{
}

void UTraderWidget::OnScrollFrontClicked()
{

}

void UTraderWidget::OnScrollBackClicked()
{

}
