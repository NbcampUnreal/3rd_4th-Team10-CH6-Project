 // Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/AttackTask.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "TimerManager.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/World.h"


 EStateTreeRunStatus UAttackTask::EnterState(FStateTreeExecutionContext& Context,
                                             const FStateTreeTransitionResult& Transition)
 {
    Super::EnterState(Context, Transition);

   if (!Actor || !TargetActor)
    {
         return EStateTreeRunStatus::Failed;
    }

    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
    {
    	//AttackSpeed = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
    }

    Actor->GetWorld()->GetTimerManager().SetTimer(
        AttackTimerHandle,
        this,
        &UAttackTask::ExecuteAttack,
        AttackSpeed,
        true,
        AttackSpeed
    );

  
    return EStateTreeRunStatus::Running;
 }

 void UAttackTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
 {
	Super::ExitState(Context, Transition);

 	if (Actor && Actor->GetWorld())
 	{
 		Actor->GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
 	}
 }

 void UAttackTask::ExecuteAttack()
 {
 	
      if (!Actor || !TargetActor)
      {
      	  if (Actor && Actor->GetWorld())
      	  {
      	  		Actor->GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
      	  }
      	
	      return;
      }

 	  if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
 	  {
	 	  FGameplayEventData EventData;
 	  	
 	  	EventData.Instigator = Actor;
 	  	EventData.Target = TargetActor;
 	  	EventData.EventTag = GASTAG::Enemy_Ability_Attack;
        
 	  	ASC->HandleGameplayEvent(
			   EventData.EventTag, 
			   &EventData
		   );
 	  }
 }
