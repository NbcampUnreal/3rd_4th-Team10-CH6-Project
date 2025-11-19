// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "Enemy_Attack_Sound_Cue.generated.h"

class USoundCue;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API UEnemy_Attack_Sound_Cue : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TMap<FGameplayTag, USoundCue*> SoundMap;

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

};
