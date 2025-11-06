#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/TraderWidget.h"
#include "TradeMainWidget.generated.h"

class UButton;
UCLASS()
class TENTENTOWN_API UTradeMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void HideeWidget();
	void ShowWidget();


	//¹öÆ°
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Section01;
	UPROPERTY(meta = (BindWidget))
	UButton* Section02;
	UPROPERTY(meta = (BindWidget))
	UButton* Section03;
	UPROPERTY(meta = (BindWidget))
	UButton* Section04;

	UPROPERTY(meta = (BindWidget))
	UButton* OffTrade;
	
	virtual void NativeConstruct() override;

public:
	void OnSection01Clicked();
	void OnSection02Clicked();
	void OnSection03Clicked();
	void OnSection04Clicked();
	void OnOffTradeClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	UTraderWidget* TraderWidget01;

public:


};
