// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/SkillTask.h"

#include "AbilitySystemGlobals.h"
#include "TTTGamePlayTags.h"
#include "Enemy/Base/EnemyBase.h"

EStateTreeRunStatus USkillTask::EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState"));

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	
	FGameplayTagContainer Tags;
	Tags.AddTag(GASTAG::Enemy_Ability_BuffNearBy);
	
	bool bActivated = ASC->TryActivateAbilitiesByTag(Tags, true);

	if (bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState Succeeded"));
		
		return EStateTreeRunStatus::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTask EnterState Failed"));
		
		return EStateTreeRunStatus::Failed;
	}

}
