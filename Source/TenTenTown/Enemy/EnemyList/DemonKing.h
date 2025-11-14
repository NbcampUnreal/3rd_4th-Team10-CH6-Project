// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Base/EnemyBase.h"
#include "DemonKing.generated.h"

UCLASS()
class TENTENTOWN_API ADemonKing : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	ADemonKing();
	UPROPERTY(EditAnywhere, Category="SPAttack")
	float BerserkHealthThreshold = 0.5f;
	
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage>BerserkMontage;
	
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage>SPAttackMontage;
	
	void CheckBerserkState();

private:
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
};
