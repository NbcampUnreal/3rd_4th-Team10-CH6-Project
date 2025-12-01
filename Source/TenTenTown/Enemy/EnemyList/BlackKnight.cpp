// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyList/BlackKnight.h"

ABlackKnight::ABlackKnight()
{
}

void ABlackKnight::InitializeEnemy()
{
	Super::InitializeEnemy();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_BlackKnight);
	}
}

