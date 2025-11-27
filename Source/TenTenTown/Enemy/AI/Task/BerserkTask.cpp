 // Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/Task/BerserkTask.h"
#include "TTTGamePlayTags.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

EStateTreeRunStatus UBerserkTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	ADemonKing* DemonKing = Cast<ADemonKing>(Actor);
	if (!DemonKing || !DemonKing->GetAbilitySystemComponent())
		return EStateTreeRunStatus::Failed;

	if (DemonKing->bBerserkPlayed)
		return EStateTreeRunStatus::Succeeded;

	if (!DemonKing->GetAbilitySystemComponent()->HasMatchingGameplayTag(GASTAG::Enemy_State_Berserk))
		return EStateTreeRunStatus::Failed;

	if (DemonKing->BerserkMontage)
	{
		FMontageEnded OnEnded;
		OnEnded.BindUFunction(this, FName("OnDeadMontageEnd"));
		UAnimInstance* AnimInstance = DemonKing->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			Duration = DemonKing->BerserkMontage->GetPlayLength();
			DemonKing->PlayMontage(DemonKing->BerserkMontage, OnEnded, 1.0f);
			DemonKing->Multicast_PlayMontage(DemonKing->BerserkMontage, 1.0f);
		}
	}

	// 최초 1회만 실행
	DemonKing->bBerserkPlayed = true;
	bHasStarted = true;
	ElapsedTime = 0.f;

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UBerserkTask::Tick(FStateTreeExecutionContext& Context, float DeltaTime)
{
	if (!bHasStarted)
		return EStateTreeRunStatus::Failed;

	ElapsedTime += DeltaTime;

	if (ElapsedTime >= Duration)
		return EStateTreeRunStatus::Succeeded;

	return EStateTreeRunStatus::Running;
}

void UBerserkTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
	bHasStarted = false;
}
