// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TTTPlayerState.generated.h"



//USTRUCT(BlueprintType)
//struct FInventoryItemData
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FName ItemName;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	int32 Count;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	int32 Level;
//};


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
	
	UFUNCTION(BlueprintCallable)
	int32 GetKillcount();
	UFUNCTION(BlueprintCallable)
	void SetKillcount(int32 NewKillcount);
	UFUNCTION(BlueprintCallable)
	void SetKillcountZero();
	UFUNCTION(BlueprintCallable)
	void AddKillcount(int32 Plus);
	
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_KillCount, meta = (AllowPrivateAccess = true))
	int32 KillCount;
	
	UFUNCTION()
	void OnRep_Gold();

	UFUNCTION()
	void OnRep_KillCount();

public:
	void AddGold(int32 Amount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddGold(int32 Amount);

	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetGold() const { return Gold; }

//protected:
//	UPROPERTY()
//	TObjectPtr<class UAS_CharacterBase> BaseAttributeSet;
public:
	//virtual void PostInitializeComponents() override;

	void OnAbilitySystemInitialized();

	UFUNCTION(Server, Reliable)
	void Server_NotifyReady();
#pragma endregion





};
