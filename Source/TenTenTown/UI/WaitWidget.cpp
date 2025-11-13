// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WaitWidget.h"
#include "Components/TextBlock.h"



void UWaitWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
void UWaitWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}


void UWaitWidget::SetWaitTime(FText NewText)
{
	UE_LOG(LogTemp, Warning, TEXT("SetWaitTime called"));
}

void UWaitWidget::SetNameText(int32 IndexNum, FText NewText)
{
	if (IndexNum == 1)
	{
		NameText1->SetText(NewText);
	}
	else if (IndexNum == 2)
	{
		NameText2->SetText(NewText);
	}
	else if (IndexNum == 3)
	{
		NameText3->SetText(NewText);
	}
}

void UWaitWidget::SetHeadImage(int32 IndexNum, UTexture2D* NewTexture)
{
	if (IndexNum == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetHeadImage 1 called"));
	}
	else if (IndexNum == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetHeadImage 2 called"));
	}
	else if (IndexNum == 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetHeadImage 3 called"));
	}
}

void UWaitWidget::SetReadyImage(int32 IndexNum, UTexture2D* NewTexture)
{
	if (IndexNum == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetReadyImage 1 called"));
	}
	else if (IndexNum == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetReadyImage 2 called"));
	}
	else if (IndexNum == 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetReadyImage 3 called"));
	}
}
