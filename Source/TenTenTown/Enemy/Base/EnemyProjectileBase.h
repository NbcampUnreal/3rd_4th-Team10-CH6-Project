// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectileBase.generated.h"

UCLASS()
class TENTENTOWN_API AEnemyProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyProjectileBase();

protected:
	virtual void BeginPlay() override;

};
