// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h" 
#include "Components/BoxComponent.h"      
#include "SpawnPoint.generated.h"

UCLASS()
class TENTENTOWN_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
    
public: 
	ASpawnPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	FName PointName;
	
	FVector GetSpawnLocation() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> SpawnArea;

};