// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "BlackKnight.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API ABlackKnight : public AEnemyBase
{
	GENERATED_BODY()
	ABlackKnight();
	
	virtual void InitializeEnemy() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage>CounterMontage;

};
