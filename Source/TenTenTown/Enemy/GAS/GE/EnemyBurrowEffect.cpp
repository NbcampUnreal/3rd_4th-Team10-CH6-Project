// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GE/EnemyBurrowEffect.h"

UEnemyBurrowEffect::UEnemyBurrowEffect()
{
	FInheritedTagContainer InheritedTags;
	InheritedTags.Added.AddTag(GASTAG::Enemy_State_Burrowed);
}
