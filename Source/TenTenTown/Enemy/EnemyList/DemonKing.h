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
	
	UPROPERTY(EditAnywhere, Category="Berserk")
	float BerserkHealthThreshold = 0.5f;

	UPROPERTY(VisibleAnywhere, Category="Berserk")
	bool bBerserkPlayed = false;
	
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage>BerserkMontage;
	
	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimMontage>SPAttackMontage;
	
	UPROPERTY(EditAnywhere, Category="GE")
	TSubclassOf<UGameplayEffect> EnemyBerserk;


private:
	virtual void InitializeEnemy() override;
	virtual void ResetEnemy() override;
	
};
