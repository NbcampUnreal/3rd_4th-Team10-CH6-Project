// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"






void USlotWidget::SetSlotWidgetData(FText NewName, UTexture2D* HeadTexture, FText NewText, int32 PriceNewText, int32 CountNewTexts)
{
	DataName = NewName;
	HeadImage->SetBrushFromTexture(HeadTexture);
	MainText->SetText(NewText);
	if (PriceNewText == -1)
	{
		PriceText->SetText(FText::FromString(TEXT("-")));
	}
	else
	{
		PriceText->SetText(FText::AsNumber(PriceNewText));
	}

	if (CountNewTexts == -1)
	{
		CountText->SetText(FText::GetEmpty());
	}
	else
	{
		CountText->SetText(FText::AsNumber(CountNewTexts));
	}
}

void USlotWidget::SetDataName(FText NewName)
{
	DataName = NewName;
}

void USlotWidget::SetHeadImage(UTexture2D* HeadTexture)
{
	HeadImage->SetBrushFromTexture(HeadTexture);
}

void USlotWidget::SetMainText(FText NewText)
{
	MainText->SetText(NewText);
}

void USlotWidget::SetPriceText(int32 PriceNewText)
{
	if (PriceNewText == -1)
	{
		PriceText->SetText(FText::FromString(TEXT("-")));
	}
	else
	{
		PriceText->SetText(FText::AsNumber(PriceNewText));
	}
}

void USlotWidget::SetCountText(int32 CountNewText)
{
	if (CountNewText == -1)
	{
		CountText->SetText(FText::GetEmpty());
	}
	else
	{
		CountText->SetText(FText::AsNumber(CountNewText));
	}
}



void USlotWidget::OnMyButtonClicked()
{
	OnSlotItemClicked.Broadcast(FText(DataName));
}
