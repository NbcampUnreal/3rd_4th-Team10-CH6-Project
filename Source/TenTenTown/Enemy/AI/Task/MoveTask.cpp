// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/Task/MoveTask.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"


EStateTreeRunStatus UMoveTask::EnterState(FStateTreeExecutionContext& Context,
                                          const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	

	Distance = Actor->MovedDistance;
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		MovementSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute());

		ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Move);
	}

	if (Actor->DistanceOffset == 0.0f)
	{
		Actor->DistanceOffset = FMath::RandRange(-SpreadDistance, SpreadDistance);
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
	if (Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		if (Actor->GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
		{
			// nullptr 전달 시 모든 몽타주 재생 여부 체크
			//몽타주 재생 중 이동 정지
			return EStateTreeRunStatus::Running;
		}
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
		FVector SplineLocation = SplineComp->GetLocationAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
		FVector Direction = SplineComp->GetDirectionAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

		const FVector RightVector = FVector::CrossProduct(Direction.GetSafeNormal(), FVector::UpVector);

		FVector OffsetVector = RightVector + FVector(Actor->DistanceOffset, 0, 0);
		FVector NewLocation = SplineLocation + OffsetVector;

		Actor->SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
		Distance = NewDistance;

		
		
		return EStateTreeRunStatus::Running;
	}
	else
	{
		return EStateTreeRunStatus::Succeeded;
	}
}

void UMoveTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	Actor->MovedDistance = Distance;

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Move);
	}
}
