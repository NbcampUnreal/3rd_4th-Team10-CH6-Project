// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyList/BlackKnight.h"

#include "Enemy/GAS/AS/BlackKnight_AttributeSet.h"
#include "Engine/Engine.h"

ABlackKnight::ABlackKnight()
{
	//PrimaryActorTick.bCanEverTick = true;
}

void ABlackKnight::InitializeEnemy()
{
	Super::InitializeEnemy();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_BlackKnight);
	}
}

void ABlackKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ASC)
	{
		if (GEngine)
		{
			FString HealthStr = FString::Printf(TEXT("BlackKnight Health: %.1f/%.1f"),
				ASC->GetNumericAttribute(UBlackKnight_AttributeSet::GetMaxHealthAttribute()),
				ASC->GetNumericAttribute(UBlackKnight_AttributeSet::GetHealthAttribute()));
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, HealthStr);
		}
	}
}

