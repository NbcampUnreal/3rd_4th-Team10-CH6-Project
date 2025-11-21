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

	// �������� ���� ID (������ ���̺��� ���� ������ ��ȸ�� �� ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	// ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count;

	// ������ �� ������ �ν��Ͻ��� ���� ������ �ٸ� �Ӽ�...
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

	/** 선택한 캐릭터 클래스 (Fighter, Mage 등) */
	UPROPERTY(ReplicatedUsing=OnRep_SelectedCharacterClass, BlueprintReadOnly, Category="Lobby")
	TSubclassOf<APawn> SelectedCharacterClass;

	/** 준비 완료 여부 */
	UPROPERTY(ReplicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category="Lobby")
	bool bIsReady = false;

	UFUNCTION()
	void OnRep_IsReady();

	/** Ready 토글 (UI에서 버튼으로 호출) */
	UFUNCTION(BlueprintCallable, Category="Lobby")
	void ToggleReady();

	/** 서버에서 Ready 값 설정 */
	UFUNCTION(Server, Reliable)
	void ServerSetReady(bool bNewReady);

	/** 편의용 Getter */
	UFUNCTION(BlueprintPure, Category="Lobby")
	bool IsReady() const { return bIsReady; }

	UFUNCTION()
	void OnRep_SelectedCharacterClass();
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GAS")
	TSubclassOf<UGameplayEffect> BaseRecoveryGE;
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

	//Ŭ�󿡼��� ������ �������� �� ��
	FInventoryItemData* FindStructureDataByName(const FText& FindItemName);
	FInventoryItemData* FindItemDataByName(const FText& FindItemName);

#pragma endregion





};
