#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/TraderWidget.h"
#include "TradeMainWidget.generated.h"

class UButton;
class UTextBlock;
UCLASS()
class TENTENTOWN_API UTradeMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void HideWidget();
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
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnSection01Clicked();
	UFUNCTION()
	void OnSection02Clicked();
	UFUNCTION()
	void OnSection03Clicked();
	UFUNCTION()
	void OnSection04Clicked();
	UFUNCTION()
	void OnOffTradeClicked();

	void SetMoneyText(int32 NewMoney);
	UTextBlock* GetMoneyText();
protected:
	UPROPERTY(meta = (BindWidget))
	UTraderWidget* TraderWidget01;
	UPROPERTY(meta = (BindWidget))
	UTraderWidget* TraderWidget02;
public:
	UTraderWidget* GetTraderWidget(int32 OutTraderNum);

	void SetTraderWidget(int32 TraderNum);
	


};
