// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FighterCharacter.generated.h"

class ATTTPlayerState;
class UGameplayAbility;
enum class ENumInputID : uint8;
class UAbilitySystemComponent;

UCLASS()
class TENTENTOWN_API AFighterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFighterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	//주요 캐싱
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PlayerState")
	TObjectPtr<ATTTPlayerState> PS;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GAS|ASC")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS|EnputIDGAMap")
	TMap <ENumInputID,TObjectPtr<UGameplayAbility>> InputIDGAMap;
	
};
