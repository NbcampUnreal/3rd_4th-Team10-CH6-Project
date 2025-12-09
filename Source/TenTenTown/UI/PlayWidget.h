#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/PartyWidget.h" 
#include "UI/SlotWidget.h"
#include "PlayWidget.generated.h"

class UPlayerStatusViewModel;
class UGameStatusViewModel;
class UPartyManagerViewModel;
class UQuickSlotManagerViewModel;
class UPartyWidget;
class USlotWidget;
class UProgressBar;
class UTextBlock;
class UImage;
class UQuickSlotBarWidget;


UCLASS()
class TENTENTOWN_API UPlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetPlayerStatusViewModel(UPlayerStatusViewModel* InViewModel);

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetPartyManagerViewModel(UPartyManagerViewModel* ViewModel);
		
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetGameStatusViewModel(UGameStatusViewModel* InViewModel);
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* InViewModel);


protected:
	virtual void NativeConstruct() override;

	
	// 플레이어 개인 스탯(체력/마나/레벨 등) 뷰모델
	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;

	// 게임 상태(웨이브/코어체력 등) 뷰모델
	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UGameStatusViewModel> GameStatusViewModel;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UPartyManagerViewModel> PartyManagerViewModel;
	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UQuickSlotManagerViewModel> QuickSlotManagerViewModel;


	// --- 바인딩 위젯 (BlueprintReadOnly로 변경하여 ViewModel 바인딩에 사용) ---

	// 플레이어 상태 표시 위젯 (자식 위젯에 ViewModel 전달 필요)
	UPROPERTY(meta = (BindWidget))
	class UImage* HeadImage; // 이 정보는 보통 ViewModel에 포함되지 않으므로, 추후 처리 필요

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar; // HealthBar는 이제 ViewModel의 HealthPercentage에 직접 바인딩됩니다.

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoreHealth; // GameStatusViewModel에 바인딩

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveTimer; // GameStatusViewModel에 바인딩

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveLevel; // GameStatusViewModel에 바인딩

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainEnemy; // GameStatusViewModel에 바인딩

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoneyText; // UPlayerStatusViewModel에 Gold 속성이 있다면 바인딩

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCounts; // UPlayerStatusViewModel에 인벤토리 정보가 있다면 바인딩

	UPROPERTY(meta = (BindWidget))
	UButton* OnTradeButton;

	// --- 파티 및 슬롯 위젯 (데이터를 받아 자식 위젯에 전달) ---
	UPROPERTY(meta = (BindWidget))
	class UListView* PartyListView;

protected:
	// 슬롯 위젯 배열은 유지 (UI 레이아웃 연결을 위해)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<USlotWidget*> UseSlotWidgets;

public:

	// 위젯 표시/숨김은 유지
	void HideWidget();
	void ShowWidget();

protected:	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UQuickSlotBarWidget> QuickSlotBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UQuickSlotBarWidget> QuickSlotBarItem;


public:
	UQuickSlotBarWidget* GetQuickSlotBarWidget() const { return QuickSlotBar; }

	void SetsPartyListView();

	UFUNCTION()
	void OnOffButtonClicked();
};