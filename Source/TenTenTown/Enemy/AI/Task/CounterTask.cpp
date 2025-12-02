// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/CounterTask.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/EnemyList/BlackKnight.h"
#include "TTTGamePlayTags.h"


void UCounterTask::ExecuteCounterAbility()
{
	if (!Actor)
	{
		return;
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		FGameplayEventData EventData;
		EventData.Instigator = Actor;
		EventData.Target = TargetActor;
		EventData.EventTag = GASTAG::Enemy_Ability_Counter; 

		ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
	}
}