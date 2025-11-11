// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/Task/SPAttackTask.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Base/EnemyBase.h"
#include "TimerManager.h"
#include "TTTGamePlayTags.h"
#include "Engine/World.h"

EStateTreeRunStatus USPAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	UE_LOG(LogTemp, Warning, TEXT("SPAttack Task Entered"));
	if (!Actor || !TargetActor)
	{
		return EStateTreeRunStatus::Failed;
	}
	bIsPlayerDetected = true;
	bCanSPAttack = true;

	
	Actor->GetWorld()->GetTimerManager().SetTimer(
		SPAttackTimerHandle,
		this,
		&USPAttackTask::ExecuteSPAttack,
		SPAttackCooldown,
		false
	);

	return EStateTreeRunStatus::Running;
}

void USPAttackTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	if (Actor && Actor->GetWorld())
	{
		Actor->GetWorld()->GetTimerManager().ClearTimer(SPAttackTimerHandle);
	}

	bIsPlayerDetected = false;
	bCanSPAttack = true; 
}

void USPAttackTask::ExecuteSPAttack()
{
	if (!bIsPlayerDetected || !bCanSPAttack || !Actor || !TargetActor)
		return;

	bCanSPAttack = false;

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		FGameplayEventData EventData;
		EventData.Instigator = Actor;
		EventData.Target = TargetActor;
		EventData.EventTag = GASTAG::Enemy_Ability_SPAttack;

		ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
	}

	// 특수공격 후 5초 쿨다운 타이머 시작
	Actor->GetWorld()->GetTimerManager().SetTimer(
		SPAttackTimerHandle,
		this,
		&USPAttackTask::ResetSPAttackCooldown,
		SPAttackCooldown,
		false
	);
}

void USPAttackTask::ResetSPAttackCooldown()
{
	bCanSPAttack = true;

	if (bIsPlayerDetected && Actor)
	{
		Actor->GetWorld()->GetTimerManager().SetTimer(
			SPAttackTimerHandle,
			this,
			&USPAttackTask::ExecuteSPAttack,
			SPAttackDelay,
			false
		);
	}
}