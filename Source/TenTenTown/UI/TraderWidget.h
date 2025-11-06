#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TraderWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;
UCLASS()
class TENTENTOWN_API UTraderWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;


	UPROPERTY(meta = (BindWidget))
	UImage* TargetItemImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDesText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemPriceText;
	UPROPERTY(meta = (BindWidget))
	UButton* BuyButton;
public:
	void SetTargetItemImage(UTexture2D* ItemTexture);
	void SetItemNameText(FName ItemName);
	void SetItemDesText(FName ItemDes);
	void SetItemPriceText(int32 ItemPrice);
	void OnBuyButtonClicked();



	//스크롤 버튼
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* ScrollFront;
	UPROPERTY(meta = (BindWidget))
	UButton* ScrollBack;
	
public:
	void OnScrollFrontClicked();
	void OnScrollBackClicked();
	
};
