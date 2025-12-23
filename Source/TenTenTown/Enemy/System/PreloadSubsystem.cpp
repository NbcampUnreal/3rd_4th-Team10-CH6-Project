// Fill out your copyright notice in the Description page of Project Settings.

#include "PreloadSubsystem.h"
#include "Engine/Engine.h"

void UPreloadSubsystem::SetupTable(UDataTable* InWaveTable)
{
    if (!InWaveTable)
    {
        return;
    }
    WaveTable = InWaveTable;
    PreloadWaveEnemies(WaveTable);
}

void UPreloadSubsystem::PreloadWaveEnemies(UDataTable* InWaveTable)
{
    if (!InWaveTable)
    {
        return;
    }
  
    TArray<FName> RowNames = InWaveTable->GetRowNames();
    for (int32 WaveIdx = 0; WaveIdx < RowNames.Num(); ++WaveIdx)
    {
        const FWaveData* WaveDataRow = InWaveTable->FindRow<FWaveData>(RowNames[WaveIdx], TEXT("Preload"));
        if (!WaveDataRow)
        {
            continue;
        }
        FEnemyWave& Wave = WavesData.FindOrAdd(WaveIdx);
        Wave.EnemyInfos = WaveDataRow->EnemyGroups;

        if (!WaveIndexTracker.Contains(WaveIdx))
        {
            WaveIndexTracker.Add(WaveIdx, 0);
        }

        for (const FEnemySpawnInfo& Info : WaveDataRow->EnemyGroups)
        {
            if (Info.EnemyBP)
            {
                UE_LOG(LogTemp, Log, TEXT("[PreloadSubsystem] Loaded: %s"), *Info.EnemyBP->GetName());
            }
        }
    }
}

bool UPreloadSubsystem::GetEnemy(int32 WaveIndex, FEnemySpawnInfo& OutEnemyInfo)
{
    if (!WavesData.Contains(WaveIndex))
    {
        return false;
    }
    int32& CurrentIdx = WaveIndexTracker.FindOrAdd(WaveIndex);
    FEnemyWave& Wave = WavesData[WaveIndex];

    if (CurrentIdx >= Wave.EnemyInfos.Num())
    {
        return false;
    }

    OutEnemyInfo = Wave.EnemyInfos[CurrentIdx];
    CurrentIdx++;

    return true;
}

void UPreloadSubsystem::RegisterSpawnedEnemy(int32 WaveIndex, AEnemyBase* Enemy)
{
    if (!WavesData.Contains(WaveIndex) || !Enemy)
    {
        return;
    }
    WavesData[WaveIndex].SpawnedEnemies.Add(Enemy);
}
