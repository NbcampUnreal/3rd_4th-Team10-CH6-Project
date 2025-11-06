// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Enemy/Base/EnemyBase.h"
#include "PoolSubsystem.generated.h"

#define INITIAL_POOL_SIZE 30

UCLASS()
class TENTENTOWN_API UPoolSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    void SetupEnemyTable(UDataTable* InEnemyTable);

    AEnemyBase* GetPooledEnemy(FName EnemyName);
    void ReleaseEnemy(AEnemyBase* Enemy);

private:
    UPROPERTY()
    UDataTable* EnemyTable = nullptr;

    TMap<FName, TArray<AEnemyBase*>> EnemyPools;

    void InitializePool();
    void DeactivateEnemy(AEnemyBase* Enemy);
};
