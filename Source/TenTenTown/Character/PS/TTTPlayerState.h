// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TTTPlayerState.generated.h"

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
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
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

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS")
	EGameplayEffectReplicationMode ReplicationMode;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UAS_MageAttributeSet> MageAttributes;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,ReplicatedUsing=OnRep_Gold,meta=(AllowPrivateAccess=true))
	int32 Gold;
	UFUNCTION()
	void OnRep_Gold();

};
