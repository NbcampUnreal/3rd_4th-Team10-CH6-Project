#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotWidget.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotItemClicked, FText, ItemName);

UCLASS()
class TENTENTOWN_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FText DataName;

	UPROPERTY(meta = (BindWidget))
	class UImage* HeadImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MainText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText;

	UPROPERTY(meta = (BindWidget))
	class UButton* SellectButton;

public:
	void SetSlotWidgetData(FText NewName, UTexture2D* HeadTexture, FText NewText, int32 PriceNewText, int32 CountNewTexts);

	void SetDataName(FText NewName);
	void SetHeadImage(UTexture2D* HeadTexture);
	void SetMainText(FText NewText);
	void SetPriceText(int32 PriceNewText);
	void SetCountText(int32 CountNewTexts);


	FText GetDataName() const { return DataName; }

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSlotItemClicked OnSlotItemClicked;

	UFUNCTION()
	void OnMyButtonClicked();
};
