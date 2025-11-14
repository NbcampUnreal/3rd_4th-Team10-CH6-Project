// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "FindClosestEvaluator.generated.h"

class AEnemyBase;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UFindClosestEvaluator : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="context")
	AEnemyBase* Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="output")
	AActor* TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="none")
	UAbilitySystemComponent* ASC;

	virtual void TreeStart(FStateTreeExecutionContext& Context) override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
};
