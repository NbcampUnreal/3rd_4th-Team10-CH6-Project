// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyList/NagaWizard.h"

void ANagaWizard::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_NagaWizard);
	}
}
