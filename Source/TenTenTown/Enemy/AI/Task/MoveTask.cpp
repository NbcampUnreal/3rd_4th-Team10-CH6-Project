// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/MoveTask.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"

EStateTreeRunStatus UMoveTask::EnterState(FStateTreeExecutionContext& Context,
                                          const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		MovementSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute());
	}
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus UMoveTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (!Actor || !SplineActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	USplineComponent* SplineComp = SplineActor->SplineActor;
	if (!SplineComp)
	{
		return EStateTreeRunStatus::Failed;
	}

	float SplineLength = SplineComp->GetSplineLength();
	float NewDistance = Distance + MovementSpeed * DeltaTime;

	if (Distance < SplineLength)
	{
		NewDistance = FMath::Min(NewDistance, SplineLength);
		FVector NewLocation = SplineComp->GetLocationAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);

		Actor->SetActorLocation(NewLocation, false);
		Distance = NewDistance;
		
		return EStateTreeRunStatus::Running;
	}
	else
	{
		return EStateTreeRunStatus::Succeeded;
	}
}
