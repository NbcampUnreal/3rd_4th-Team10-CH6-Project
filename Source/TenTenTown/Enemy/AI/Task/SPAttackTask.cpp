// Fill out your copyright notice in the Description page of Project Settings.

#include "SPAttackTask.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "AbilitySystemGlobals.h"
#include "TimerManager.h"
#include "TTTGamePlayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

EStateTreeRunStatus USPAttackTask::EnterState(FStateTreeExecutionContext& Context,
                                              const FStateTreeTransitionResult& Transition)
{
    Super::EnterState(Context, Transition);
    ADemonKing* DemonKing = Cast<ADemonKing>(Actor);
    
    if (!DemonKing || !TargetActor)
        return EStateTreeRunStatus::Failed;

    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(DemonKing))
    {
        AttackSpeed = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
    }

   
    DemonKing->GetWorld()->GetTimerManager().SetTimer(
        SPAttackTimerHandle,
        this,
        &USPAttackTask::ExecuteSPAttack,
        AttackSpeed/2,
        true,
        0.5f
    );

    return EStateTreeRunStatus::Running;
}

void USPAttackTask::ExitState(FStateTreeExecutionContext& Context,
                              const FStateTreeTransitionResult& Transition)
{
    Super::ExitState(Context, Transition);
    ADemonKing* DemonKing = Cast<ADemonKing>(Actor);

    if (DemonKing && DemonKing->GetWorld())
    {
        DemonKing->GetWorld()->GetTimerManager().ClearTimer(SPAttackTimerHandle);
    }
}

void USPAttackTask::ExecuteSPAttack()
{
    ADemonKing* DemonKing = Cast<ADemonKing>(Actor);
    if (!DemonKing || !TargetActor)
        return;

    ExecuteRotate();

  
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(DemonKing))
    {
        FGameplayEventData EventData;
        EventData.Instigator = DemonKing;
        EventData.Target = TargetActor;
        EventData.EventTag = GASTAG::Enemy_Ability_Attack;

        ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
    }
    
    if (DemonKing->SPAttackMontage)
    {
        DemonKing->PlayMontage(DemonKing->SPAttackMontage, FMontageEnded(), 1.0f);
        DemonKing->Multicast_PlayMontage(DemonKing->SPAttackMontage, 1.0f);
    }
}

void USPAttackTask::ExecuteRotate()
{
    if (!Actor || !TargetActor)
        return;

    FVector TargetLocation = TargetActor->GetActorLocation();
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation);
    FRotator NewRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

    Actor->SetActorRotation(NewRotation);
}
