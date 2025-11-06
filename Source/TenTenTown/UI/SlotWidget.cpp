// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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
