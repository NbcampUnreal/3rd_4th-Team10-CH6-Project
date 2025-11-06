#include "UI/TradeMainWidget.h"
#include "Components/Button.h"

void UTradeMainWidget::HideeWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UTradeMainWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UTradeMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Section01)
	{
		Section01->OnClicked.AddDynamic(this, &UTradeMainWidget::OnSection01Clicked);
	}
	if (Section02)
	{
		Section02->OnClicked.AddDynamic(this, &UTradeMainWidget::OnSection02Clicked);
	}
	if (Section03)
	{
		Section03->OnClicked.AddDynamic(this, &UTradeMainWidget::OnSection03Clicked);
	}
	if (Section04)
	{
		Section04->OnClicked.AddDynamic(this, &UTradeMainWidget::OnSection04Clicked);
	}
	if (OffTrade)
	{
		OffTrade->OnClicked.AddDynamic(this, &UTradeMainWidget::OnOffTradeClicked);
	}
}

void UTradeMainWidget::OnSection01Clicked()
{

}

void UTradeMainWidget::OnSection02Clicked()
{

}

void UTradeMainWidget::OnSection03Clicked()
{

}

void UTradeMainWidget::OnSection04Clicked()
{

}

void UTradeMainWidget::OnOffTradeClicked()
{
	HideeWidget();
	//컨트롤 변경
}
