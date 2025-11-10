// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "Enemy/Data/WaveData.h"
#include "SpawnSubsystem.generated.h"

class ASpawnPoint;
class AEnemyBase;

struct FSpawnTask
{
	FEnemySpawnInfo Info;
	int32 SpawnedCount = 0;
	FTimerHandle TimerHandle;

	FSpawnTask(const FEnemySpawnInfo& InInfo) : Info(InInfo) {}
};

UCLASS()
class TENTENTOWN_API USpawnSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void SetupWaveTable(TSoftObjectPtr<UDataTable> InWaveData);
	void StartWave(int32 WaveIndex);

private:
	void SpawnEnemy(FName EnemyName, FName SpawnPointName);
	ASpawnPoint* FindSpawnPointByName(FName PointName);
	void HandleSpawnTick(FSpawnTask* SpawnTask);

	UPROPERTY()
	UDataTable* WaveTable;

	TArray<FSpawnTask*> ActiveSpawnTasks;
};
