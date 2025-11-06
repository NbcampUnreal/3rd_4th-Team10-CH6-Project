#include "UI/PlayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayWidget::SetHealthPercent(float Percent)
{
	HealthBar->SetPercent(Percent);
}

void UPlayWidget::SetCoreHealth(int32 HealthPoint)
{
	CoreHealth->SetText(FText::AsNumber(HealthPoint));
}

void UPlayWidget::SetWaveTimer(int32 WaveTimeCount)
{
	WaveTimer->SetText(FText::AsNumber(WaveTimeCount));
}

void UPlayWidget::SetWaveLevel(int32 WaveLevelCount)
{
	WaveLevel->SetText(FText::AsNumber(WaveLevelCount));
}

void UPlayWidget::SetRemainEnemy(int32 RemainEnemyCount)
{
	RemainEnemy->SetText(FText::AsNumber(RemainEnemyCount));
}

void UPlayWidget::SetMoneyText(int32 MoneyCount)
{
	MoneyText->SetText(FText::AsNumber(MoneyCount));
}

void UPlayWidget::SetItemCounts(int32 ItemCount)
{
	ItemCounts->SetText(FText::AsNumber(ItemCount));
}

void UPlayWidget::SetPartyWidget(int32 IndexNum, FText NameText, UTexture2D* HeadTexture, float HealthPercent)
{
	if (IndexNum == 1)
	{
		PartyWidget01->SetNameText(NameText);
		PartyWidget01->SetTargetItemImage(HeadTexture);
		PartyWidget01->SetHealthBar(HealthPercent);
	}
	else if (IndexNum == 2)
	{
		PartyWidget02->SetNameText(NameText);
		PartyWidget02->SetTargetItemImage(HeadTexture);
		PartyWidget02->SetHealthBar(HealthPercent);
	}
	else if (IndexNum == 3)
	{
		PartyWidget03->SetNameText(NameText);
		PartyWidget03->SetTargetItemImage(HeadTexture);
		PartyWidget03->SetHealthBar(HealthPercent);
	}
}

void UPlayWidget::SetUseSlotWidget(int32 IndexNum, UTexture2D* ItemTexture, FText MainNewText,int32 ItemPriceText)
{
	if (IndexNum >= 1 && UseSlotWidgets.IsValidIndex(IndexNum - 1))
	{
		USlotWidget* TargetSlotWidget = UseSlotWidgets[IndexNum - 1];

		if (TargetSlotWidget)
		{
			TargetSlotWidget->SetCountText(IndexNum);
			TargetSlotWidget->SetHeadImage(ItemTexture);
			TargetSlotWidget->SetMainText(MainNewText);
			TargetSlotWidget->SetPriceText(ItemPriceText);
		}
	}
}
