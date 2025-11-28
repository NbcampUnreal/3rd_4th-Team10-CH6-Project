// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnSubsystem.h"
#include "EngineUtils.h"
#include "PoolSubsystem.h"
#include "Enemy/Base/EnemyBase.h"
#include "SpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Enemy/Data/WaveData.h"
#include "Engine/World.h"

void USpawnSubsystem::SetupTable(TSoftObjectPtr<UDataTable> InWaveData)
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

void USpawnSubsystem::SpawnEnemy(const FEnemySpawnInfo& EnemyInfo)
{
    UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>();
    if (!PoolSubsystem)
    {
        return;
    }
    AEnemyBase* Enemy = PoolSubsystem->GetPooledEnemy(EnemyInfo);
    if (!Enemy)
    {
        return;
    }
    ASpawnPoint* SpawnPoint = FindSpawnPointByName(EnemyInfo.SpawnPoint);
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

    //무한 스폰 설정 시
    if (SpawnTask->Info.bInfiniteSpawn)
    {
        // 게임모드 Combat Phase 종료 전까지 무한 반복
        SpawnEnemy(SpawnTask->Info);
    }
    //스폰 수 제한 설정 시
    else
    {
        
        if (SpawnTask->SpawnedCount < SpawnTask->Info.SpawnCount)
        {
            //스폰 수 만큼 반복
            SpawnEnemy(SpawnTask->Info);
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
}
void USpawnSubsystem::EndWave()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    
    for (FSpawnTask* Task : ActiveSpawnTasks)
    {
        World->GetTimerManager().ClearTimer(Task->TimerHandle);
        delete Task;
    }
    
    ActiveSpawnTasks.Empty();

    //월드에 존재하는 모든 EnemyBase 검색
    for (TActorIterator<AEnemyBase> It(World); It; ++It)
    {
        AEnemyBase* Enemy = *It;
        if (!Enemy)
        {
            continue;
        }
        if (UPoolSubsystem* Pool = World->GetSubsystem<UPoolSubsystem>())
        {
            Pool->ReleaseEnemy(Enemy);
        }
    }
}
