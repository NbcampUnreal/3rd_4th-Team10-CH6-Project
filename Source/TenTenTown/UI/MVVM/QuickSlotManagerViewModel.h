#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "QuickSlotManagerViewModel.generated.h"


class UQuickSlotEntryViewModel;


UCLASS()
class TENTENTOWN_API UQuickSlotManagerViewModel : public UBaseViewModel
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UInventoryPCComponent> CachedInventory;


public:
	
	void InitializeViewModel(ATTTPlayerState* InPlayerState);

	
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
	TArray<TObjectPtr<UQuickSlotEntryViewModel>> QuickSlotEntryVMs;

	UFUNCTION()
	void OnQuickSlotListChanged(const TArray<FInventoryItemData>& NewQuickSlotList);

	//UFUNCTION(BlueprintPure, Category = "QuickSlot")
	const TArray<TObjectPtr<UQuickSlotEntryViewModel>>& GetQuickSlotEntryVMs() const
	{
		return QuickSlotEntryVMs;
	}

	

private:
	void CreateQuickSlotEntries();
	
};
