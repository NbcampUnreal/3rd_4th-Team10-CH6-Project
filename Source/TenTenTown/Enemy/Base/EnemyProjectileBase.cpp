// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Base/EnemyProjectileBase.h"

// Sets default values
AEnemyProjectileBase::AEnemyProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AEnemyProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

