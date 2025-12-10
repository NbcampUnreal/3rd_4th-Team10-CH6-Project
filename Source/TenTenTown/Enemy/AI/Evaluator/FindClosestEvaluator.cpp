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
			if (NearestTarget != TargetActor) // 타겟이 변경되었거나 새로 설정됨
			{
				//UnbindCurrentTarget(); // 기존 타겟 해제
				TargetActor = NearestTarget; // 새 타겟 설정
				//BindToTargetDeath(TargetActor); // 새 타겟 바인딩
				UE_LOG(LogTemp, Warning, TEXT("FindClosestEvaluator TargetActor: %s"), *TargetActor->GetName());
			}
		}
		else
		{
			TargetActor = nullptr;
		
		}
		
	}
	
}

void UFindClosestEvaluator::OnCurrentTargetDied(const FOnAttributeChangeData& Data)
{
}

void UFindClosestEvaluator::BindToTargetDeath(AActor* NewTarget)
{
}

void UFindClosestEvaluator::UnbindCurrentTarget()
{
}
