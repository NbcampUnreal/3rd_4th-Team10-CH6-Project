#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/PartyWidget.h"
#include "UI/SlotWidget.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "PlayWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;

UCLASS()
class TENTENTOWN_API UPlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	

	virtual void NativeConstruct() override;

	
	
	
	


	

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* HeadImage;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	
public:
	void SetHealthPercent(float HealthPer);
	void HideWidget();
	void ShowWidget();
	

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoreHealth;
public:
	void SetCoreHealth(int32 HealthPoint);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveTimer;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveLevel;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainEnemy;

	

public:
	void SetWaveTimer(int32 WaveTimeCount);
	void SetWaveLevel(int32 WaveLevelCount);
	void SetRemainEnemy(int32 RemainEnemyCount);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCounts;
public:
	void SetMoneyText(int32 MoneyCount);
	void SetItemCounts(int32 ItemCount);


protected:
	UPROPERTY(meta = (BindWidget))
	UPartyWidget* PartyWidget01;
	UPROPERTY(meta = (BindWidget))
	UPartyWidget* PartyWidget02;
	UPROPERTY(meta = (BindWidget))
	UPartyWidget* PartyWidget03;

public:
	void SetPartyWidget(int32 IndexNum, FText NameText, UTexture2D* HeadTexture, float HealthPercent);


protected:

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<USlotWidget*> UseSlotWidgets;

	/*UPROPERTY(meta = (BindWidget))
	USlotWidget* UseSlot01;
	UPROPERTY(meta = (BindWidget))
	USlotWidget* UseSlot02;
	UPROPERTY(meta = (BindWidget))
	USlotWidget* UseSlot03;
	UPROPERTY(meta = (BindWidget))
	USlotWidget* UseSlot04;
	UPROPERTY(meta = (BindWidget))
	USlotWidget* UseSlot05;
	UPROPERTY(meta = (BindWidget))
	USlotWidget* UseSlot06;
	UPROPERTY(meta = (BindWidget))
	USlotWidget* UseSlot07;*/

public:
	void SetUseSlotWidget(int32 IndexNum, UTexture2D* ItemTexture, FText MainNewText, int32 ItemPriceText);
};
