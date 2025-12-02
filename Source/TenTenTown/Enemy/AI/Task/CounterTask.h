// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackTask.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "CounterTask.generated.h"

/**
 * 
 */
class AEnemyBase;

UCLASS()
class TENTENTOWN_API UCounterTask : public UAttackTask
{
	GENERATED_BODY()

	void ExecuteCounterAbility();
};