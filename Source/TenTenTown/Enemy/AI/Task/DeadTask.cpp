// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/DeadTask.h"

#include "TimerManager.h"
#include "Enemy/Base/EnemyBase.h"
#include "Engine/Engine.h"

EStateTreeRunStatus UDeadTask::EnterState(FStateTreeExecutionContext& Context,
                                          const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	if (Actor)
	{
		bMontageEnded = false;
		
		FMontageEnded OnEnded;
		OnEnded.BindUFunction(this, FName("OnDeadMontageEnd"));

		GEngine->AddOnScreenDebugMessage(
		-1,                 
		5.0f,               
		FColor::Yellow,     
		FString::Printf(TEXT("Dead"))
		);
		
		Actor->PlayMontage(Actor->DeadMontage, OnEnded, 1.0f);

		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus UDeadTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (!Actor)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (bMontageEnded && Actor->GetLocalRole() == ROLE_Authority)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;

}

void UDeadTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
                               const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

void UDeadTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	if (Actor && Actor->GetLocalRole() == ROLE_Authority)
	{
		if (Actor)
		{
			Actor->DropGoldItem();

			// 풀 실행
			
			Actor->SetLifeSpan(0.1f);

		}
	}
}

void UDeadTask::OnDeadMontageEnd(UAnimMontage* Montage)
{
	if (Actor && Actor->GetLocalRole() == ROLE_Authority)
	{
		bMontageEnded = true;
	}
}

