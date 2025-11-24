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
public:
	void ReBeginPlay();

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
	void InitializeViewModels();

	FTimerHandle HUDOpenTimerHandle;
public:
	// GameState 델리게이트 핸들러 (GameStateRef가 델리게이트를 브로드캐스트한다고 가정)
	void HandlePhaseChanged(ETTTGamePhase NewPhase);
	void HandleRemainingTimeChanged(int32 NewRemainingTime);

	UPartyManagerViewModel* GetPartyManagerViewModel() const { return PartyManagerViewModel; }

	// -----------------------------------------------------------------------------------
	// GAS 콜백 함수 제거: ViewModel이 Attribute 변화를 직접 구독합니다.
	// UPlayPCComponent는 더 이상 이 변화를 직접 처리할 필요가 없습니다.
	// -----------------------------------------------------------------------------------
	// void OnHealthChanged(const FOnAttributeChangeData& Data);
	// void OnStaminaChanged(const FOnAttributeChangeData& Data);
	// void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
};