// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyList/Worm.h"

void AWorm::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_Worm);
	}
}
