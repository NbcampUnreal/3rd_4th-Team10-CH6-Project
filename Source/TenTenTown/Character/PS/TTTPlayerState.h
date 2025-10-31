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
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS")
	EGameplayEffectReplicationMode ReplicationMode;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,ReplicatedUsing=OnRep_Gold,meta=(AllowPrivateAccess=true))
	int32 Gold;
	UFUNCTION()
	void OnRep_Gold();
};
