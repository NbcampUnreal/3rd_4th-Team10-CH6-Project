#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/PartyWidget.h" 
#include "UI/SlotWidget.h"
#include "PlayWidget.generated.h"

class UPlayerStatusViewModel;
class UGameStatusViewModel;
class UPartyManagerViewModel;
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
	// PCC에서 호출하여 ViewModel을 설정하는 함수 (플레이어 스탯)
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetPlayerStatusViewModel(UPlayerStatusViewModel* InViewModel);

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetPartyManagerViewModel(UPartyManagerViewModel* ViewModel);

	// PCC에서 호출하여 ViewModel을 설정하는 함수 (게임 상태)
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void SetGameStatusViewModel(UGameStatusViewModel* InViewModel);


protected:
	virtual void NativeConstruct() override;

	// --- MVVM 바인딩 소스 ---

	// 플레이어 개인 스탯(체력/마나/레벨 등) 뷰모델
	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;

	// 게임 상태(웨이브/코어체력 등) 뷰모델
	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UGameStatusViewModel> GameStatusViewModel;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<UPartyManagerViewModel> PartyManagerViewModel;


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


	// --- 파티 및 슬롯 위젯 (데이터를 받아 자식 위젯에 전달) ---

	/*UPROPERTY(meta = (BindWidget))
	UPartyWidget* PartyWidget01;
	UPROPERTY(meta = (BindWidget))
	UPartyWidget* PartyWidget02;
	UPROPERTY(meta = (BindWidget))
	UPartyWidget* PartyWidget03;*/
	UPROPERTY(meta = (BindWidget))
	class UListView* PartyListView;

protected:
	// 슬롯 위젯 배열은 유지 (UI 레이아웃 연결을 위해)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<USlotWidget*> UseSlotWidgets;

	/* 기존 주석 처리된 BindWidget 슬롯 변수들은 제거하거나,
	   UseSlotWidgets 배열에 연결하는 로직으로 NativeConstruct에서 처리해야 합니다. */

public:
	// **MVVM 전환으로 인해 Set 함수는 더 이상 필요하지 않습니다.**
	// **ViewModel의 속성에 직접 바인딩되어 데이터가 자동으로 갱신됩니다.**
	/*
	void SetHealthPercent(float HealthPer);
	void SetCoreHealth(int32 HealthPoint);
	void SetWaveTimer(int32 WaveTimeCount);
	void SetWaveLevel(int32 WaveLevelCount);
	void SetRemainEnemy(int32 RemainEnemyCount);
	void SetMoneyText(int32 MoneyCount);
	void SetItemCounts(int32 ItemCount);
	void SetPartyWidget(int32 IndexNum, FText NameText, UTexture2D* HeadTexture, float HealthPercent);
	void SetUseSlotWidget(int32 IndexNum, UTexture2D* ItemTexture, FText MainNewText, int32 ItemPriceText);
	*/

	// 위젯 표시/숨김은 유지
	void HideWidget();
	void ShowWidget();

protected:
	// UMG 디자이너에서 배치하고 'Is Variable'로 만든 퀵슬롯 바 위젯 변수
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UQuickSlotBarWidget> QuickSlotBar; // UMG 변수 이름은 반드시 'QuickSlotBar'여야 합니다!

public:
	// PlayPCComponent가 위젯 인스턴스를 가져갈 수 있도록 Getter 추가
	UQuickSlotBarWidget* GetQuickSlotBarWidget() const { return QuickSlotBar; }
};