// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "SpawnSubsystem.generated.h"

class ASpawnPoint;
class AEnemyBase;

UCLASS()
class TENTENTOWN_API USpawnSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void SetupWaveTable(TSoftObjectPtr<UDataTable> InWaveData);
	void StartWave(int32 WaveIndex);

private:
	UPROPERTY()
	UDataTable* WaveTable = nullptr;
	UPROPERTY()
	TArray<FTimerHandle> ActiveSpawnTimers;

	void SpawnEnemy(FName EnemyName, FName SpawnPointName);
	ASpawnPoint* FindSpawnPointByName(FName PointName);
};
