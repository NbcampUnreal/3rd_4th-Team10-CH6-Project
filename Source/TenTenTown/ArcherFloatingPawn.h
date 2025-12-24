// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ArcherFloatingPawn.generated.h"

class UUserWidget;
class UCameraComponent;
class USphereComponent;
class USpringArmComponent;
class UInputMappingContext;
struct FInputActionValue;
class UInputAction;
class UFloatingPawnMovement;

UCLASS()
class TENTENTOWN_API AArcherFloatingPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AArcherFloatingPawn();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovementComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UDecalComponent> DecalComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Material")
	TObjectPtr<UMaterialInterface> DecalMaterial;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="InputActions")
	TObjectPtr<UInputMappingContext> IMCGhost;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="InputActions")
	TObjectPtr<UInputAction> ConfirmAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="InputActions")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="InputActions")
	TObjectPtr<UInputAction> AscendAction;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Crosshair")
	TSubclassOf<UUserWidget> CrosshairClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> CrosshairWidget;
	
public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& Value);
	void Ascend(const FInputActionValue& Value);
	void ConfirmLocation(const FInputActionValue& Value);
};
