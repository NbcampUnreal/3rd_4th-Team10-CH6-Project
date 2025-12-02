#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "UObject/WeakObjectPtr.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "QuickSlotEntryViewModel.generated.h"


class ATTTPlayerState;
class UTexture2D;

UCLASS()
class TENTENTOWN_API UQuickSlotEntryViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
public:    
    void InitializeViewModel();

    void CleanupViewModel();

    // 현재 설치 개수 / 최대 설치 개수
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CountText;

    // 설치 비용
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CostText;

    // 아이템 아이콘
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    TObjectPtr<UTexture2D> IconTexture;

    // 현재 보유 재화로 설치 가능한지 여부..?
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    bool bCanAfford = false;

protected:
    TWeakObjectPtr<ATTTPlayerState> PlayerStateWeakPtr;
    

public:
    int32 SlotIndex = INDEX_NONE;
    int32 GetSlotIndex() const { return SlotIndex; }



private:
    // FieldNotify Setter 함수 정의
    void SetCountText(const FText& NewValue);
    void SetCostText(const FText& NewValue);
    void SetIconTexture(UTexture2D* NewValue);
    void SetCanAfford(bool bNewValue);

public:
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    int32 SlotNumber = 0; // 초기값 0-> 이 후 고정(VM에서 수행)

    void SetSlotNumber(int32 NewNumber);


public:
    void UpdateItemData(const FInventoryItemData& NewData);
    void ClearItemData();

    void BroadcastAllFieldValues();

};
