// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/SkillTask.h"

#include "AbilitySystemGlobals.h"
#include "TTTGamePlayTags.h"
#include "Enemy/Base/EnemyBase.h"

EStateTreeRunStatus USkillTask::EnterState(FStateTreeExecutionContext& Context,
                                           const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	
	FGameplayTagContainer Tags;
	Tags.AddTag(GASTAG::Enemy_Ability_BuffNearBy.GetTag());
	
	ASC->TryActivateAbilitiesByTag(Tags, true);

	return EStateTreeRunStatus::Running;
}
