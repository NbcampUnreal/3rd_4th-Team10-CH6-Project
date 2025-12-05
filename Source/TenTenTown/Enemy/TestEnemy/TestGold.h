// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestGold.generated.h"

class URotatingMovementComponent;

UCLASS()
class TENTENTOWN_API ATestGold : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestGold();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RotateComponent")
	TObjectPtr<URotatingMovementComponent> RotateMovementComponent;
	
	UFUNCTION()
    void OnHitCallback(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;

public:	

};
