#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PartyWidget.generated.h"


UCLASS()
class TENTENTOWN_API UPartyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* HeadImage;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText;

public:
	void SetTargetItemImage(UTexture2D* HeadTexture);
	void SetHealthBar(float HealthPer);
	void SetNameText(FText NewText);
};
