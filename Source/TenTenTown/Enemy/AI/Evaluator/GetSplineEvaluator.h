// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "GetSplineEvaluator.generated.h"

class AEnemyBase;
class ASplineActor;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UGetSplineEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="context")
	TObjectPtr<AEnemyBase> Actor;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="output")
	TObjectPtr<ASplineActor> SplineActor;
	
public:
	void virtual TreeStart(FStateTreeExecutionContext& Context) override;

	
};
