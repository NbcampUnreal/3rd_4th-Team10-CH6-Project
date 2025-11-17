// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Enemy/EnemyList/DemonKing.h"
#include "Engine/TimerHandle.h"
#include "SPAttackTask.generated.h"

UCLASS()
class TENTENTOWN_API USPAttackTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

private:
	FTimerHandle SPAttackTimerHandle;

	float AttackSpeed = 1.f;

	bool bHasPlayedBerserkMontage = false;

	void ExecuteSPAttack();
	void ExecuteRotate();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Context))
	AEnemyBase* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Input))
	AActor* TargetActor;
	

	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
										   const FStateTreeTransitionResult& Transition) override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
						   const FStateTreeTransitionResult& Transition) override;
};
