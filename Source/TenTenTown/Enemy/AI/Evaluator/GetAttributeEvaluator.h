// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "Enemy/Base/EnemyBase.h"
#include "GetAttributeEvaluator.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGetAttributeEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	TObjectPtr<AEnemyBase> Enemy;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="output")
	float MovementSpeed;
	

public:
	void virtual Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
};
