// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "FighterCharacter.generated.h"

class UInteractionSystemComponent;
class UAS_FighterAttributeSet;
class UAttributeSet;
struct FInputActionInstance;
class UCameraComponent;
class USpringArmComponent;
class ATTTPlayerState;
class UInputAction;
class UInputMappingContext;
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
	//인풋 액션
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputMappingContext> IMC;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> JumpAction;

	//인풋 액션 바인딩 함수
	void Move(const FInputActionInstance& FInputActionInstance);
	void Look(const FInputActionInstance& FInputActionInstance);
	void ActivateGAByInputID(const FInputActionInstance& FInputActionInstance,ENumInputID InputID);
	
	//InputID, GA
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS|EnputIDGAMap")
	TMap <ENumInputID,TSubclassOf<UGameplayAbility>> InputIDGAMap;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GAS|Attributeset")
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;
	//기본 컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basci Components")
	TObjectPtr<UInteractionSystemComponent> ISC;
	//주요 캐싱
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PlayerState")
	TObjectPtr<ATTTPlayerState> PS;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GAS|ASC")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GAS|AS")
	const UAS_FighterAttributeSet* FighterAttributeSet;
	
};
