// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnSubsystem.h"
#include "PoolSubsystem.h"
#include "Enemy/Base/EnemyBase.h"
#include "SpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Enemy/Data/WaveData.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void USpawnSubsystem::SetupWaveTable(TSoftObjectPtr<UDataTable> InWaveData)
{
    WaveTable = InWaveData.LoadSynchronous();
    if (!WaveTable){
        UE_LOG(LogTemp, Error, TEXT("SpawnSubsystem: Failed to load Wave DataTable"));
    }
}
void USpawnSubsystem::StartWave(int32 WaveIndex)
{
    if (!WaveTable)
    {
        return;
    }
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    FName TargetWaveName = *FString::FromInt(WaveIndex); 
    const FString Context = TEXT("StartWave");

    TArray<FName> RowNames = WaveTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {

        const FWaveData* WaveData = WaveTable->FindRow<FWaveData>(RowName, Context);
        if (!WaveData || WaveData->Wave != TargetWaveName)
        {
            continue;
        }

        for (const FEnemySpawnInfo& Info : WaveData->EnemyGroups)
        {
            int32 SpawnedCount = 0;
            FTimerHandle SpawnTimerHandle;

            FEnemySpawnInfo SpawnInfo = Info;

            World->GetTimerManager().SetTimer(
                SpawnTimerHandle,
                FTimerDelegate::CreateLambda([this, SpawnTimerHandle, SpawnInfo, SpawnedCount, World]() mutable
                {
                    if (SpawnedCount < SpawnInfo.SpawnCount)
                    {
                        SpawnEnemy(SpawnInfo.EnemyName, SpawnInfo.SpawnPoint);
                        SpawnedCount++;
                    }
                    else
                    {
                        World->GetTimerManager().ClearTimer(SpawnTimerHandle);
                    }
                }),
                SpawnInfo.SpawnInterval,
                true,
                SpawnInfo.SpawnDelay
            );
        }

        break;
    }
}


void USpawnSubsystem::SpawnEnemy(FName EnemyName, FName SpawnPointName)
{
    
    UPoolSubsystem* PoolSubsystem = GetGameInstance()->GetSubsystem<UPoolSubsystem>();
    if (!PoolSubsystem)
    {
        return;
    }
    AEnemyBase* Enemy = PoolSubsystem->GetPooledEnemy(EnemyName);
    if (!Enemy)
    {
        return;
    }
    ASpawnPoint* SpawnPoint = FindSpawnPointByName(SpawnPointName);
    if (!SpawnPoint)
    {
        PoolSubsystem->ReleaseEnemy(Enemy);
        return;
    }

    Enemy->SetActorLocation(SpawnPoint->GetSpawnLocation());
    Enemy->SetActorHiddenInGame(false);
    Enemy->SetActorEnableCollision(true);
    Enemy->SetActorTickEnabled(true);

}

//스폰할 포인트를 이름으로 탐색
ASpawnPoint* USpawnSubsystem::FindSpawnPointByName(FName PointName)
{
    //월드 내 스폰 포인트를 배열에 저장
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (ASpawnPoint* Point = Cast<ASpawnPoint>(Actor))
        {
            if (Point->PointName == PointName)
            {
                return Point;
            }
        }
    }
    return nullptr;
}