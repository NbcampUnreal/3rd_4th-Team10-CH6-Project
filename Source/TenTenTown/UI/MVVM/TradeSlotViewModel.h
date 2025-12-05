// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Item/Data/ItemData.h"
#include "Engine/Texture2D.h"
#include "Character/PS/TTTPlayerState.h"
#include "TradeSlotViewModel.generated.h"



UCLASS()
class TENTENTOWN_API UTradeSlotViewModel : public UBaseViewModel
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<ATTTPlayerState> PlayerStateWeakPtr;
    

    // 소유 / 최대
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CountText;

    // 구매 비용
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CostText;

    // 아이템 아이콘
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    TObjectPtr<UTexture2D> IconTexture;

    // 현재 보유 재화로 설치 가능한지 여부..?
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    bool bCanAfford = false;

public:
    FText ItemName;

	void SetSlotItem(const FItemData& NewItemData);
	void OnSetCountText(const FItemData& NewItemData);

    void SetCountText(const FText& NewValue);
    void SetCostText(const FText& NewValue);
    void SetIconTexture(UTexture2D* NewValue);
	
    void BroadcastAllFieldValues();
    void SetBindingDelegate();
};
