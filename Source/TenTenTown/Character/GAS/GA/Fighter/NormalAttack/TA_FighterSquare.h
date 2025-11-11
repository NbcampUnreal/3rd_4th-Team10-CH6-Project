// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TA_FighterSquare.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API ATA_FighterSquare : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

	ATA_FighterSquare();
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargeting() override;
	virtual void CancelTargeting() override;
	
};
