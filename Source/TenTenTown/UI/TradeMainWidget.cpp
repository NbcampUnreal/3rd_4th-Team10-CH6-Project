#include "UI/TradeMainWidget.h"
#include "Components/Button.h"
#include "UI/PlayHUD.h"
#include "Components/TextBlock.h"


void UTradeMainWidget::HideWidget()
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
		OnSection01Clicked();
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
	SetTraderWidget(1);
}

void UTradeMainWidget::OnSection02Clicked()
{
	SetTraderWidget(2);
}

void UTradeMainWidget::OnSection03Clicked()
{

}

void UTradeMainWidget::OnSection04Clicked()
{

}

void UTradeMainWidget::OnOffTradeClicked()
{
    APlayerController* PlayerController = GetOwningPlayer();

    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnOffTradeClicked: PlayerController가 유효하지 않습니다."));
        return;
    }

    AHUD* CurrentHUD = PlayerController->GetHUD();

    if (!CurrentHUD)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnOffTradeClicked: 현재 HUD가 유효하지 않습니다."));
        return;
    }

    //APlayHUD* PlayHUD = Cast<APlayHUD>(CurrentHUD);

    /*if (PlayHUD)
    {
        PlayHUD->OpenTradeWidget(false);

        UE_LOG(LogTemp, Log, TEXT("OnOffTradeClicked: PlayHUD의 OpenTradeWidget() 호출 성공."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnOffTradeClicked: HUD를 APlayHUD로 캐스팅하는 데 실패했습니다."));
    }*/
	
}

UTraderWidget* UTradeMainWidget::GetTraderWidget(int32 OutTraderNum)
{
	if (OutTraderNum == 1)
	{
		return TraderWidget01;
	}
	else if (OutTraderNum == 2)
	{
		return TraderWidget02;
	}


	return nullptr;
}

void UTradeMainWidget::SetTraderWidget(int32 TraderNum)
{
	if (TraderNum == 1)
	{
		TraderWidget02->HideWidget();
		TraderWidget01->ShowWidget();
	}
	else if (TraderNum == 2)
	{
		TraderWidget01->HideWidget();
		TraderWidget02->ShowWidget();
	}
}

void UTradeMainWidget::SetMoneyText(int32 NewMoney)
{
	if (MoneyText)
	{
		MoneyText->SetText(FText::AsNumber(NewMoney));
	}
}

UTextBlock* UTradeMainWidget::GetMoneyText()
{

	return MoneyText;
}
