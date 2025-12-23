// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/Data/WaveData.h"
#include "Subsystems/WorldSubsystem.h"
#include "PreloadSubsystem.generated.h"

#define BOSS_PRELOAD_SIZE 1

USTRUCT()
struct FEnemyArray
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<AEnemyBase*> Enemies;
};

USTRUCT()
struct FEnemyWave
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<TSubclassOf<AEnemyBase>, FEnemyArray> Waves;
};

UCLASS()
class TENTENTOWN_API UPreloadSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void SetupTable(UDataTable* InWaveTable);

    AEnemyBase* GetEnemy(int32 WaveIndex, const FEnemySpawnInfo& EnemyInfo);


private:
    UPROPERTY()
    UDataTable* WaveTable = nullptr;

    UPROPERTY()
    TMap<int32, FEnemyWave> PreloadedEnemies;

    void InitializeEnemies();
};
