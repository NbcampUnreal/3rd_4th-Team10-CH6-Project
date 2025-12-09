// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/Data/WaveData.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolSubsystem.generated.h"

#define INITIAL_POOL_SIZE 20
#define BOSS_POOL_SIZE 1
UCLASS()
class TENTENTOWN_API UPoolSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void SetupTable(UDataTable* InWaveTable);

    AEnemyBase* GetPooledEnemy(int32 WaveIndex, const FEnemySpawnInfo& EnemyInfo);

    void ReleaseEnemy(int32 WaveIndex, AEnemyBase* Enemy);

private:
    UPROPERTY()
    UDataTable* WaveTable = nullptr;

    TMap<int32, TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>>> EnemyPools;

    void InitializePool();
    void DeactivateEnemy(AEnemyBase* Enemy);
};
