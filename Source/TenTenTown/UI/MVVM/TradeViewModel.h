#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Item/Data/ItemData.h"
#include "Engine/Texture2D.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "UI/MVVM/TradeSlotViewModel.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "TradeViewModel.generated.h"

UCLASS()
class TENTENTOWN_API UTradeViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
protected:
	/*UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "UI|Trade")
	int32 PlayerGold;*/
	/*UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "UI|Trade")	
	TObjectPtr<UTexture2D> TargetImage;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "UI|Trade")
	FText TargetName;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "UI|Trade")
	FText TargetDes;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Category = "UI|Trade")
	FText TargetPrice;*/

	
public:

	//void SetPlayerGold(int32 NewGold);
	/*void SetTargetImage(UTexture2D* NewTexture2D);
	void SetTargetName(FText& NewName);
	void SetTargetDes(FText& NewDes);
	void SetTargetPrice(FText& NewPrice);*/

	//void SetHeadItem(FItemData& NewItemData);
	

protected:

	UPROPERTY()
	TObjectPtr<UInventoryPCComponent> CachedInventory;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	TArray<TObjectPtr<UTradeSlotViewModel>> TradeSlotEntryVMs;
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	bool CanTrade = false;

	UPROPERTY()
	TObjectPtr<UPlayPCComponent> CachedPlayPCComponent;
	UPROPERTY()
	TObjectPtr<UTradeSlotViewModel> TradeHeadSlotMV;


public:
	UInventoryPCComponent* GetInventoryPCComponent() const;

	void InitializeViewModel(UPlayPCComponent* CachedPlayPCC, ATTTPlayerState* InPlayerState, UTTTGameInstance* TTGI);
	void CreateTradeSlotEntries(UTTTGameInstance* TTGI);
	//void TradeSlotListBased(FItemData& NewItemData);
	void OnTradeSlotListChanged(const TArray<FInventoryItemData>& NewQuickSlotList);

	UFUNCTION()
	TArray<UTradeSlotViewModel*> GetPartyMembers() const;

	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void CallSlotDelegate();

	UFUNCTION()
	void OnInventoryUpdated(const TArray<FItemInstance>& NewItems);

	UFUNCTION()
	void SetTradeHeadSlotMV(UTradeSlotViewModel* NewTradeHeadSlotMV);

	UFUNCTION()
	void SetCanTrade(int32 golds);
	
};
