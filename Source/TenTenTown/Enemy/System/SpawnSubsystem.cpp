// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnSubsystem.h"
#include "EngineUtils.h"
#include "PreloadSubsystem.h"
#include "Enemy/Base/EnemyBase.h"
#include "SpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Enemy/Data/WaveData.h"
#include "Engine/World.h"
#include "GameSystem/GameMode/TTTGameModeBase.h"

void USpawnSubsystem::SetupTable(TSoftObjectPtr<UDataTable> InWaveData)
{
    WaveTable = InWaveData.LoadSynchronous();
    if (!WaveTable){
        UE_LOG(LogTemp, Error, TEXT("SpawnSubsystem: Failed to load Wave DataTable"));
    }
}

/*void USpawnSubsystem::StartWave(int32 WaveIndex)
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
}*/
void USpawnSubsystem::StartWave(int32 WaveIndex)
{
    if (!WaveTable) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FName TargetWaveName = *FString::FromInt(WaveIndex);
    const FString Context = TEXT("StartWave");

    bool bIsBossWave = false;
    
    if (ATTTGameModeBase* GM = Cast<ATTTGameModeBase>(UGameplayStatics::GetGameMode(World)))
    {
        bIsBossWave = GM->IsBossWave(WaveIndex); //보스 웨이브 확인
    }
    // =========================
    // ★ 1) 이번 웨이브 총 스폰 수 계산 (테이블 기반)
    // =========================
    int32 TotalSpawnCount = 0;
    bool bHasInfinite = false;

    TArray<FName> RowNames = WaveTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FWaveData* WaveData = WaveTable->FindRow<FWaveData>(RowName, Context);
        if (!WaveData || WaveData->Wave != TargetWaveName) continue;

        for (const FEnemySpawnInfo& Info : WaveData->EnemyGroups)
        {
            if (Info.bInfiniteSpawn)
            {
                bHasInfinite = true;
            }
            else if (!Info.bIsBoss)//보스는 카운트 제외
            {
                TotalSpawnCount += Info.SpawnCount;
            }
        }
    }

    // =========================
    // ★ 2) GameMode에 Target 세팅 (이게 없으면 페이즈 절대 안 넘어감)
    // =========================
    if (ATTTGameModeBase* GM = Cast<ATTTGameModeBase>(UGameplayStatics::GetGameMode(World)))
    {
        if (bHasInfinite)
        {
            UE_LOG(LogTemp, Error, TEXT("[SpawnSubsystem] Wave %d has bInfiniteSpawn=true. KillCount clear cannot work on this wave."), WaveIndex);
            // 그냥 0이면 TryClearPhaseByKillCount가 비교 자체를 안 함 → 끝나지 않는 게 정상
            GM->SetPhaseTargetKillCount(0);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[SpawnSubsystem] Wave %d TotalSpawnCount=%d -> SetPhaseTargetKillCount"), WaveIndex, TotalSpawnCount);
            GM->SetPhaseTargetKillCount(TotalSpawnCount);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SpawnSubsystem] GameMode is not ATTTGameModeBase. Cannot set target kill count."));
    }

    // =========================
    // 3) 기존 스폰 타이머 세팅
    // =========================
    for (const FName& RowName : RowNames)
    {
        const FWaveData* WaveData = WaveTable->FindRow<FWaveData>(RowName, Context);
        if (!WaveData || WaveData->Wave != TargetWaveName) continue;

        for (const FEnemySpawnInfo& Info : WaveData->EnemyGroups)
        {
            if (Info.bIsBoss)//컴뱃 페이즈에서만 일반 몬스터 스폰
            {
                continue;
            }
            FSpawnTask* NewTask = new FSpawnTask(WaveIndex,Info);
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

void USpawnSubsystem::SpawnBoss(int32 WaveIndex)
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
    const FString Context = TEXT("SpawnBossForWave");
    FName TargetWaveName = *FString::FromInt(WaveIndex);

    TArray<FName> RowNames = WaveTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        const FWaveData* WaveData = WaveTable->FindRow<FWaveData>(RowName, Context);
        if (!WaveData || WaveData->Wave != TargetWaveName) continue;

        for (const FEnemySpawnInfo& Info : WaveData->EnemyGroups)
        {
            if (!Info.bIsBoss) continue; // 보스만 스폰
            SpawnEnemy(WaveIndex,Info);
        }
    }

}

void USpawnSubsystem::SpawnEnemy(int32 WaveIndex, const FEnemySpawnInfo& EnemyInfo)
{
    UPreloadSubsystem* PreloadSubsystem = GetWorld()->GetSubsystem<UPreloadSubsystem>();
    if (!PreloadSubsystem)
    {
        return;
    }
    AEnemyBase* Enemy = PreloadSubsystem->GetEnemy(WaveIndex, EnemyInfo);
    if (!Enemy)
    {
        return;
    }
   ASpawnPoint* SpawnPoint = FindSpawnPointByName(EnemyInfo.SpawnPoint);
    if (!SpawnPoint)
    {
      //  Enemy->Destroy();
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
    if (!SpawnTask)
    {
        return;
    }
    //무한 스폰 설정 시
    if (SpawnTask->Info.bInfiniteSpawn)
    {
        // 게임모드 Combat Phase 종료 전까지 무한 반복
        SpawnEnemy(SpawnTask->WaveIndex, SpawnTask->Info);
    }
    //스폰 수 제한 설정 시
    else
    {
        
        if (SpawnTask->SpawnedCount < SpawnTask->Info.SpawnCount)
        {
            //스폰 수 만큼 반복
            SpawnEnemy(SpawnTask->WaveIndex, SpawnTask->Info);
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
void USpawnSubsystem::EndWave(int32 WaveIndex)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    
    for (FSpawnTask* Task : ActiveSpawnTasks)
    {
        World->GetTimerManager().ClearTimer(Task->TimerHandle);
    }
    for (FSpawnTask* Task : ActiveSpawnTasks)
    {
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
      
        Enemy->Destroy();
        
    }
}
