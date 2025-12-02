// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "BlackKnight_AttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UBlackKnight_AttributeSet : public UAS_EnemyAttributeSetBase
{
	GENERATED_BODY()

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

};
