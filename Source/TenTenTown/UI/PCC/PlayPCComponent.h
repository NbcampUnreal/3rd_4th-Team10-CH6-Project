#pragma once

#include "CoreMinimal.h"
#include "UI/PCC/PCCBase.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "UI/MVVM/GameStatusViewModel.h"
#include "UI/MVVM/PartyManagerViewModel.h"
#include "PlayPCComponent.generated.h"


class UPlayerStatusViewModel;
class UPlayWidget;
class UTradeMainWidget;
class UDataTable;
class ATTTPlayerState;
class UQuickSlotManagerViewModel;
class UQuickSlotBarWidget;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TENTENTOWN_API UPlayPCComponent : public UPCCBase
{
	GENERATED_BODY()

public:
	UPlayPCComponent();

protected:
	virtual void BeginPlay() override;
	//void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	FTimerHandle InitCheckTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UPlayWidget> PlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayWidget> PlayWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UTradeMainWidget> TradeMainWidgetClass;

	UPROPERTY()
	TObjectPtr<UTradeMainWidget> TradeMainWidgetInstance;

	UPROPERTY()
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;
	UPROPERTY()
	TObjectPtr<UGameStatusViewModel> GameStatusViewModel;
	UPROPERTY()
	TObjectPtr<UPartyManagerViewModel> PartyManagerViewModel;

	UPROPERTY()
	TObjectPtr<UQuickSlotManagerViewModel> QuickSlotManagerViewModel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UQuickSlotBarWidget> QuickSlotBarWidgetClass;
	UPROPERTY()
	TObjectPtr<UQuickSlotBarWidget> QuickSlotBarWidgetInstance;

public:
	void RemoveStartWidget();
	void OpenTrader(bool BIsOpen);

protected:
	// UPCCBase에서 정의된 함수를 오버라이드하여 HUD 켜기/끄기 로직을 처리합니다.
	virtual void OnModeTagChanged(const FGameplayTag Tag, int32 NewCount) override;

public:
	// Gameplay Tag에 의해 호출되는 UI 생명주기 관리 함수들
	void OpenHUDUI();
	void CloseHUDUI();
private:
	void InitializeQuickSlotSystem();


	FTimerHandle TimerHandle_OpenHUD;

	// 지연된 시간 후 실행될 함수 선언
	void DelayedOpenHUDUI();

protected:
	UPROPERTY()
	ATTTGameStateBase* GameStateRef;

	UPROPERTY(BlueprintReadOnly, Category = "PlayerState")
	ATTTPlayerState* PlayerStateRef;

	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> MyASC;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* StructureDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* ItemDataTable;
	
	FTimerHandle HUDOpenTimerHandle;
public:
	// GameState 델리게이트 핸들러 (GameStateRef가 델리게이트를 브로드캐스트한다고 가정)
	void HandlePhaseChanged(ETTTGamePhase NewPhase);
	void HandleRemainingTimeChanged(int32 NewRemainingTime);

	UPartyManagerViewModel* GetPartyManagerViewModel() const { return PartyManagerViewModel; }

	
protected:
	

private:
	FTimerHandle ReBeginPlayTimerHandle;

	FTimerHandle SetASCTimerHandle;
	void FindSetASC();
	FTimerHandle OpenReadyTimerHandle;
	void CallOpenReadyUI();

	FTimerHandle TestTimerHandle;
	void TestFunction();

	FTimerHandle RefreshTimerHandle;
	void ForceRefreshList();

public:
	FTimerHandle TestTimerHandle2;
	void TestFunction2();

	UPROPERTY(EditDefaultsOnly, Category = "GAS Debug")
	TSubclassOf<class UGameplayEffect> DebugDamageGEClass;

	UFUNCTION(Server, Reliable)
	void ServerApplyDamageGE();

	FTimerHandle TestTimerHandle3;
	void TestFunction3();
};