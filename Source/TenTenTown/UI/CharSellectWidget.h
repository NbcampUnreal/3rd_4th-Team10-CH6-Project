#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharSellectWidget.generated.h"

class UButton;
UCLASS()
class TENTENTOWN_API UCharSellectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* WarriorButton;
	UPROPERTY(meta = (BindWidget))
	UButton* MageButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ArcherButton;
	UPROPERTY(meta = (BindWidget))
	UButton* RogueButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;


public:
	void HideWidget();
	void ShowWidget();


public:
	UFUNCTION()
	void OnWarriorButtonClicked();
	UFUNCTION()
	void OnMageButtonClicked();
	UFUNCTION()
	void OnArcherButtonClicked();
	UFUNCTION()
	void OnRogueButtonClicked();
	UFUNCTION()
	void OnConfirmButtonClicked();
};
