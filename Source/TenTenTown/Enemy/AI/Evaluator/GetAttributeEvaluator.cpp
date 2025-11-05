// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/AI/Evaluator/GetAttributeEvaluator.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include "DynamicMesh/DynamicMesh3.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"

void UGetAttributeEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Enemy))
	{
		MovementSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute());
	}
}
