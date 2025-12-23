// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/Data/WaveData.h"
#include "PreloadSubsystem.generated.h"

USTRUCT()
struct FEnemyWave
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FEnemySpawnInfo> EnemyInfos;//스폰할 몬스터 정보

    UPROPERTY()
    TArray<AEnemyBase*> SpawnedEnemies;//스폰된 몬스터 관리
};

UCLASS()
class TENTENTOWN_API UPreloadSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    //게임모드에서 데이터 테이블 받아오기
    void SetupTable(UDataTable* InWaveTable);
    //데이터 테이블에서 웨이브별 몬스터 데이터 저장
    void PreloadWaveEnemies(UDataTable* InWaveTable);
    //웨이브에서 스폰할 몬스터 데이터 
    bool GetEnemy(int32 WaveIndex, FEnemySpawnInfo& OutEnemyInfo);
    //스폰된 몬스터 등록
    void RegisterSpawnedEnemy(int32 WaveIndex, AEnemyBase* Enemy);

    UPROPERTY()
    TMap<int32, FEnemyWave> WavesData;

private:
    UPROPERTY()
    UDataTable* WaveTable = nullptr;
    
    // 각 웨이브에서 스폰할 EnemyInfo 인덱스
    TMap<int32, int32> WaveIndexTracker;
};
