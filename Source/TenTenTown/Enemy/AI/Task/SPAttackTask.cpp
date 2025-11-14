#include "SPAttackTask.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "AbilitySystemGlobals.h"
#include "TimerManager.h"
#include "TTTGamePlayTags.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

EStateTreeRunStatus USPAttackTask::EnterState(FStateTreeExecutionContext& Context,
                                              const FStateTreeTransitionResult& Transition)
{
    Super::EnterState(Context, Transition);

    if (!Actor || !TargetActor)
        return EStateTreeRunStatus::Failed;

    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
    {
        AttackSpeed = ASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
    }

    // SP 공격 반복 실행 타이머 설정
    Actor->GetWorld()->GetTimerManager().SetTimer(
        SPAttackTimerHandle,
        this,
        &USPAttackTask::ExecuteSPAttack,
        AttackSpeed,
        true,
        SPAttackDelay
    );

    return EStateTreeRunStatus::Running;
}

void USPAttackTask::ExitState(FStateTreeExecutionContext& Context,
                              const FStateTreeTransitionResult& Transition)
{
    Super::ExitState(Context, Transition);

    if (Actor && Actor->GetWorld())
    {
        Actor->GetWorld()->GetTimerManager().ClearTimer(SPAttackTimerHandle);
    }
}

void USPAttackTask::ExecuteSPAttack()
{
    if (!Actor || !TargetActor)
        return;

    ExecuteRotate();

    // SP 공격 이벤트 전송
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
    {
        FGameplayEventData EventData;
        EventData.Instigator = Actor;
        EventData.Target = TargetActor;
        EventData.EventTag = GASTAG::Enemy_Ability_Attack;

        ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
    }

    // SP 공격 몽타주는 그대로 재생 (광폭화 여부와 무관하게)
    if (Actor->SPAttackMontage)
    {
        Actor->PlayMontage(Actor->SPAttackMontage, FMontageEnded(), 1.0f);
        Actor->Multicast_PlayMontage(Actor->SPAttackMontage, 1.0f);
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
