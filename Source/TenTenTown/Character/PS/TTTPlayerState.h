// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TTTPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStructureListChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemListChanged);


USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	// 아이템의 고유 ID (데이터 테이블에서 세부 정보를 조회할 때 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	// 현재 소유 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count;

	// 내구도 등 아이템 인스턴스별 변동 가능한 다른 속성...
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;
};

enum class EGameplayEffectReplicationMode : uint8;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API ATTTPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATTTPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; };
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS")
	EGameplayEffectReplicationMode ReplicationMode;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UAS_MageAttributeSet> MageAttributes;


#pragma region UI Using

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Gold, meta = (AllowPrivateAccess = true))
	int32 Gold;
	UFUNCTION()
	void OnRep_Gold();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryStructure, meta = (AllowPrivateAccess = true))
	TArray<FInventoryItemData> StructureList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryItem, meta = (AllowPrivateAccess = true))
	TArray<FInventoryItemData> ItemList;

	UFUNCTION()
	void OnRep_InventoryStructure();
	UFUNCTION()
	void OnRep_InventoryItem();


public:
	UPROPERTY(BlueprintAssignable, Category = "UI Updates")
	FOnGoldChanged OnGoldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "UI Updates")
	FOnStructureListChanged OnStructureListChangedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "UI Updates")
	FOnItemListChanged OnItemListChangedDelegate;

	void AddGold(int32 Amount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddGold(int32 Amount);

	UFUNCTION(Server, Reliable)
	void Server_UpdateStructureData(const FInventoryItemData& NewStructureData);
	UFUNCTION(Server, Reliable)
	void Server_UpdateItemData(const FInventoryItemData& NewItemData);

	//클라에서는 정보를 변경하지 말 것
	FInventoryItemData* FindStructureDataByName(const FText& FindItemName);
	FInventoryItemData* FindItemDataByName(const FText& FindItemName);

#pragma endregion




};
