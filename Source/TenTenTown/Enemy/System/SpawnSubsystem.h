// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enemy/Data/WaveData.h"
#include "Subsystems/WorldSubsystem.h"
#include "SpawnSubsystem.generated.h"

class ASpawnPoint;
class AEnemyBase;

struct FSpawnTask
{
	FEnemySpawnInfo Info;
	int32 SpawnedCount = 0;
	int32 WaveIndex = 0;
	FTimerHandle TimerHandle;

	FSpawnTask(int32 InWaveIndex,const FEnemySpawnInfo& InInfo )  : Info(InInfo), WaveIndex(InWaveIndex){}
};

UCLASS()
class TENTENTOWN_API USpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void SetupTable(TSoftObjectPtr<UDataTable> InWaveData);
	void StartWave(int32 WaveIndex);
	void SpawnBoss(int32 WaveIndex);
	void EndWave(int32 WaveIndex);
private:
	void SpawnEnemy(int32 WaveIndex,const FEnemySpawnInfo& EnemyInfo);
	ASpawnPoint* FindSpawnPointByName(FName PointName);
	void HandleSpawnTick(FSpawnTask* SpawnTask);

	UPROPERTY()
	UDataTable* WaveTable = nullptr;

	TArray<FSpawnTask*> ActiveSpawnTasks;
};
