#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotWidget.generated.h"

class UTextBlock;
UCLASS()
class TENTENTOWN_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* HeadImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MainText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountText;

public:
	void SetHeadImage(UTexture2D* HeadTexture);
	void SetMainText(FText NewText);
	void SetPriceText(int32 PriceNewText);
	void SetCountText(int32 CountNewTexts);
};
