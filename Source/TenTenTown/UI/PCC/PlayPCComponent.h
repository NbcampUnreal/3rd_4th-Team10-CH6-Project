#pragma once

#include "CoreMinimal.h"
#include "UI/PCC/PCCBase.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "UI/MVVM/GameStatusViewModel.h"
#include "UI/MVVM/PartyManagerViewModel.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "PlayPCComponent.generated.h"


class UPlayerStatusViewModel;
class UPlayWidget;
class UTradeMainWidget;
class UDataTable;
class ATTTPlayerState;
class UQuickSlotManagerViewModel;
class UQuickSlotBarWidget;
class UTradeViewModel;
class USkillCoolTimeViewModel;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TENTENTOWN_API UPlayPCComponent : public UPCCBase
{
	GENERATED_BODY()

public:
	UPlayPCComponent();

protected:
	virtual void BeginPlay() override;

protected:
	FTimerHandle InitCheckTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UPlayWidget> PlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayWidget> PlayWidgetInstance;

	//»óÁ¡
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UTradeMainWidget> TradeMainWidgetClass;
	UPROPERTY()
	TObjectPtr<UTradeMainWidget> TradeMainWidgetInstance;

	//ºä¸ðµ¨
	UPROPERTY()
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;
	UPROPERTY()
	TObjectPtr<UGameStatusViewModel> GameStatusViewModel;
	UPROPERTY()
	TObjectPtr<UPartyManagerViewModel> PartyManagerViewModel;
	UPROPERTY()
	TObjectPtr<UQuickSlotManagerViewModel> QuickSlotManagerViewModel;
	UPROPERTY()
	TObjectPtr<UTradeViewModel> TradeViewModel;
	UPROPERTY()
	TObjectPtr<USkillCoolTimeViewModel> SkillCoolTimeViewModel;

public:
	void RemoveStartWidget();
	void OpenTrader(bool BIsOpen);
	UTradeMainWidget* GetTradeMainWidgetInstance() const;

protected:
	virtual void OnModeTagChanged(const FGameplayTag Tag, int32 NewCount) override;

public:
	void OpenHUDUI();
	void CloseHUDUI();

private:
	FTimerHandle TimerHandle_OpenHUD;


protected:
	UPROPERTY()
	ATTTGameStateBase* GameStateRef;

	UPROPERTY(BlueprintReadOnly, Category = "PlayerState")
	ATTTPlayerState* PlayerStateRef;

	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> MyASC;
	UPROPERTY(BlueprintReadOnly, Category = "PlayerCharacter")
	TObjectPtr<ABaseCharacter> MyCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* StructureDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* ItemDataTable;
	
	FTimerHandle HUDOpenTimerHandle;
public:
	void HandlePhaseChanged(ETTTGamePhase NewPhase);
	void HandleRemainingTimeChanged(int32 NewRemainingTime);

	UPartyManagerViewModel* GetPartyManagerViewModel() const { return PartyManagerViewModel; }

	ATTTPlayerState* GetPCCPlayerStateRef() const { return PlayerStateRef; }
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

	UFUNCTION()
	void OnShopOpenTagChanged(const FGameplayTag Tag, int32 NewCount);

	void UpdateInputMode();


	UFUNCTION(Server, Reliable)
	void Server_ControllTradeOpenEffect(bool OnOff);

	
};