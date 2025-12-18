// Fill out your copyright notice in the Description page of Project Settings.


#include "FindClosestEvaluator.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Base/EnemyBase.h"

void UFindClosestEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);

}

void UFindClosestEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Combat))
	{
		const TArray<TWeakObjectPtr<AActor>>& OverlappingList = Actor->GetOverlappedPawns();

		AActor* NearestTarget = nullptr;
		float MinDistanceSq = FLT_MAX;
		FVector OwnerLocation = Actor->GetActorLocation();

		for (const TWeakObjectPtr<AActor>& TargetPtr : OverlappingList)
		{
			if (AActor* CurrentActor = TargetPtr.Get())
			{
				float DistanceSq = FVector::DistSquared(OwnerLocation, CurrentActor->GetActorLocation());
            
				if (DistanceSq < MinDistanceSq)
				{
					MinDistanceSq = DistanceSq;
					NearestTarget = CurrentActor;
				}
			}
		}

		if (NearestTarget)
		{
			if (NearestTarget != TargetActor)
			{
				TargetActor = NearestTarget;
			}
		}
		else
		{
			TargetActor = nullptr;
		
		}
		
	}
	
}
