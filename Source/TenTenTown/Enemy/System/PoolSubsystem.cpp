// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolSubsystem.h"
#include "Enemy/Data/EnemyData.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"

// 데이터 테이블 설정 후 초기화
void UPoolSubsystem::SetupEnemyTable(UDataTable* InEnemyTable)
{
    if (!InEnemyTable)
    {
        return;
    }

    EnemyTable = InEnemyTable;
    InitializePool();
}

// 풀 초기화 
void UPoolSubsystem::InitializePool()
{
    if (!EnemyTable || !GetWorld())
    {
        return;
    }

    TArray<FName> RowNames = EnemyTable->GetRowNames();

    for (FName RowName : RowNames)
    {
        const FEnemyData* Data = EnemyTable->FindRow<FEnemyData>(RowName, TEXT(""));
        if (!Data || !Data->EnemyBP)
        {
            continue;
        }

        TArray<AEnemyBase*>& Pool = EnemyPools.FindOrAdd(Data->EnemyName);
        if (Pool.Num() > 0)
        {
            continue;
        }

        //enemy 비활성화 후 풀에 추가
        for (int32 i = 0; i < INITIAL_POOL_SIZE; ++i)
        {
            if (AEnemyBase* Enemy = GetWorld()->SpawnActor<AEnemyBase>(Data->EnemyBP))
            {
                DeactivateEnemy(Enemy);
                Pool.Add(Enemy);
            }
        }
    }
}

// 풀에서 Enemy 획득, 없으면 런타임 생성
AEnemyBase* UPoolSubsystem::GetPooledEnemy(FName EnemyName)
{
    if (TArray<AEnemyBase*>* Pool = EnemyPools.Find(EnemyName))
    {
        if (Pool->Num() > 0)
        {
            return Pool->Pop();
        }
    }

    if (!EnemyTable || !GetWorld())
    {
        return nullptr;
    }

    const FEnemyData* Data = EnemyTable->FindRow<FEnemyData>(EnemyName, TEXT(""));
    if (!Data || !Data->EnemyBP)
    {
        return nullptr;
    }

    if (AEnemyBase* Enemy = GetWorld()->SpawnActor<AEnemyBase>(Data->EnemyBP))
    {
        DeactivateEnemy(Enemy);
        return Enemy;
    }

    return nullptr;
}

// 사용된 Enemy 풀에 반환
void UPoolSubsystem::ReleaseEnemy(AEnemyBase* Enemy)
{
    if (!Enemy || !EnemyTable)
    {
        return;
    }

    DeactivateEnemy(Enemy);

    TArray<FName> RowNames = EnemyTable->GetRowNames();
    FName PoolKey;

    for (FName RowName : RowNames)
    {
        const FEnemyData* Data = EnemyTable->FindRow<FEnemyData>(RowName, TEXT(""));
        if (Data)
        {
            if (Data->EnemyBP == Enemy->GetClass())
            {
                PoolKey = Data->EnemyName;
                break;
            }
        }
    }

    EnemyPools.FindOrAdd(PoolKey).Add(Enemy);
}

// Enemy 비활성화
void UPoolSubsystem::DeactivateEnemy(AEnemyBase* Enemy)
{
    if (!Enemy)
    {
        return;
    }

    if (AController* Controller = Enemy->GetController())
    {
        Controller->StopMovement();
    }
    
    Enemy->SetActorHiddenInGame(true);
    Enemy->SetActorEnableCollision(false);
    Enemy->SetActorTickEnabled(false);
    Enemy->SetActorLocation(FVector::ZeroVector);
}
