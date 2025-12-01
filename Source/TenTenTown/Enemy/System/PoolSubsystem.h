// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enemy/Base/EnemyBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolSubsystem.generated.h"

#define INITIAL_POOL_SIZE 50

UCLASS()
class TENTENTOWN_API UPoolSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    void SetupTable(UDataTable* InWaveTable);

    AEnemyBase* GetPooledEnemy(const struct FEnemySpawnInfo& EnemyInfo);

    void ReleaseEnemy(AEnemyBase* Enemy);

private:
    UPROPERTY()
    UDataTable* WaveTable = nullptr;

    TMap<FName, TArray<AEnemyBase*>> EnemyPools;

    void InitializePool();
    void DeactivateEnemy(AEnemyBase* Enemy);
};
