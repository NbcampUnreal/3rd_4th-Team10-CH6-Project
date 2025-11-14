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
    if (!WaveTable) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FName TargetWaveName = *FString::FromInt(WaveIndex);
    const FString Context = TEXT("StartWave");

    TArray<FName> RowNames = WaveTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FWaveData* WaveData = WaveTable->FindRow<FWaveData>(RowName, Context);
        if (!WaveData || WaveData->Wave != TargetWaveName) continue;

        for (const FEnemySpawnInfo& Info : WaveData->EnemyGroups)
        {
            FSpawnTask* NewTask = new FSpawnTask(Info);
            ActiveSpawnTasks.Add(NewTask);

            World->GetTimerManager().SetTimer(
                NewTask->TimerHandle,
                FTimerDelegate::CreateUObject(this, &USpawnSubsystem::HandleSpawnTick, NewTask),
                Info.SpawnInterval,
                true,
                Info.SpawnDelay
            );
        }
    }
}

void USpawnSubsystem::SpawnEnemy(FName EnemyName, FName SpawnPointName)
{
    UPoolSubsystem* PoolSubsystem = GetGameInstance()->GetSubsystem<UPoolSubsystem>();
    if (!PoolSubsystem) return;

    AEnemyBase* Enemy = PoolSubsystem->GetPooledEnemy(EnemyName);
    if (!Enemy) return;

    ASpawnPoint* SpawnPoint = FindSpawnPointByName(SpawnPointName);
    if (!SpawnPoint)
    {
        PoolSubsystem->ReleaseEnemy(Enemy);
        return;
    }

    FTransform SpawnTransform = SpawnPoint->GetSpawnTransform();
    Enemy->SetActorLocation(SpawnTransform.GetLocation());
    Enemy->SetActorRotation(SpawnTransform.GetRotation());
    Enemy->SetActorHiddenInGame(false);
    Enemy->SetActorEnableCollision(true);
    Enemy->SetActorTickEnabled(true);

    Enemy->StartTree();
}

ASpawnPoint* USpawnSubsystem::FindSpawnPointByName(FName PointName)
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        if (ASpawnPoint* Point = Cast<ASpawnPoint>(Actor))
        {
            if (Point->PointName == PointName) return Point;
        }
    }
    return nullptr;
}

void USpawnSubsystem::HandleSpawnTick(FSpawnTask* SpawnTask)
{
    if (!SpawnTask) return;

    if (SpawnTask->SpawnedCount < SpawnTask->Info.SpawnCount)
    {
        SpawnEnemy(SpawnTask->Info.EnemyName, SpawnTask->Info.SpawnPoint);
        SpawnTask->SpawnedCount++;
    }
    else
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(SpawnTask->TimerHandle);
        }

        ActiveSpawnTasks.RemoveSingleSwap(SpawnTask);
        delete SpawnTask;
    }
}
