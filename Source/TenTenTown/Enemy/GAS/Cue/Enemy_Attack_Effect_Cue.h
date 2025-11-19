// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "Enemy_Attack_Effect_Cue.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Attack_Effect_Cue : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float Offset = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TMap<FGameplayTag, UNiagaraSystem*> EffectMap;

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

	
};
