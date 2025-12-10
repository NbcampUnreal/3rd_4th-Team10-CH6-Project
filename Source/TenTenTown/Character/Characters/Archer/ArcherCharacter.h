// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "ArcherCharacter.generated.h"

class AArcherBow;
/**
 * 
 */
UCLASS()
class TENTENTOWN_API AArcherCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AArcherCharacter();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="BowActor")
	TSubclassOf<AArcherBow> BowClass;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="BowActor")
	TObjectPtr<AArcherBow> EquippedBow;
	
private:
	void EquipBow();
};
