// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/AI/Task/AttackTask.h"
#include "SPAttackTask.generated.h"

UCLASS()
class TENTENTOWN_API USPAttackTask : public UAttackTask
{
	GENERATED_BODY()

private:
	FTimerHandle SPAttackTimerHandle;
	bool bCanSPAttack = true;
	bool bIsPlayerDetected = false;
	
	UPROPERTY(EditAnywhere, Category="Boss|Attack")
	float SPAttackDelay = 5.f;
	UPROPERTY(EditAnywhere, Category="Boss|Attack")
	float SPAttackCooldown = 5.f;

	void ExecuteSPAttack();
	void ResetSPAttackCooldown();

public:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};