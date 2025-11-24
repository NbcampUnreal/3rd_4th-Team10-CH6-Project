// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolSubsystem.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Data/WaveData.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

// 데이터 테이블 설정 후 초기화
void UPoolSubsystem::SetupTable(UDataTable* InWaveTable)
{
    if (!InWaveTable)
    {
        return;
    }
    WaveTable = InWaveTable;
    InitializePool();
}
//풀 초기화
void UPoolSubsystem::InitializePool()
{
    if (!WaveTable || !GetWorld())
    {
        return;
    }
    TArray<FName> RowNames = WaveTable->GetRowNames();
    for (FName RowName : RowNames)
    {
        const FWaveData* Data = WaveTable->FindRow<FWaveData>(RowName, TEXT("InitializePool"));
        if (!Data)
        {
            continue;
        }
        for (const FEnemySpawnInfo& EnemyInfo : Data->EnemyGroups)
        {
            if (!EnemyInfo.EnemyBP)
            {
                continue;
            }
            TArray<AEnemyBase*>& Pool = EnemyPools.FindOrAdd(EnemyInfo.EnemyName);
            if (Pool.Num() > 0) continue;

            for (int32 i = 0; i < INITIAL_POOL_SIZE; ++i)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AEnemyBase* Enemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyInfo.EnemyBP, FVector(0.f, 0.f, -10000.f), FRotator::ZeroRotator, SpawnParams);
                if (!Enemy)
                {
                    continue;
                }
                Enemy->SpawnDefaultController();

                DeactivateEnemy(Enemy);
                Pool.Add(Enemy);
            }
        }
    }
}
//풀에서 Enemy 획득
AEnemyBase* UPoolSubsystem::GetPooledEnemy(const FEnemySpawnInfo& EnemyInfo)
{
    TArray<AEnemyBase*>* Pool = EnemyPools.Find(EnemyInfo.EnemyName);
    if (!Pool || Pool->Num() == 0)
    {
        return nullptr;
    }
    AEnemyBase* Enemy = Pool->Pop();
    if (!Enemy)
    {
        return nullptr;
    }
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Enemy))
    {
        if (const UAS_EnemyAttributeSetBase* AttrSet = ASC->GetSet<UAS_EnemyAttributeSetBase>())
        {
            float Mult = EnemyInfo.StatMultiplier;
            ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute(), AttrSet->GetMaxHealth() * Mult);
            ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetHealthAttribute(), AttrSet->GetHealth() * Mult);
            ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute(), AttrSet->GetAttack() * Mult);
        }
    }
    
    return Enemy;
}

// 사용된 Enemy 풀에 반환
void UPoolSubsystem::ReleaseEnemy(AEnemyBase* Enemy)
{
    if (!Enemy || !WaveTable)
    {
        return;
    }
    DeactivateEnemy(Enemy);

    TArray<FName> RowNames = WaveTable->GetRowNames();
    FName PoolKey;
    for (FName RowName : RowNames)
    {
        const FWaveData* Data = WaveTable->FindRow<FWaveData>(RowName, TEXT(""));
        if (!Data)
        {
            continue;
        }
        for (const FEnemySpawnInfo& EnemyInfo : Data->EnemyGroups)
        {
            if (EnemyInfo.EnemyBP.Get() == Enemy->GetClass())
            {
                PoolKey = EnemyInfo.EnemyName;
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
        Controller->Destroy();
    }
    
    Enemy->SetActorHiddenInGame(true);
    Enemy->SetActorEnableCollision(false);
    Enemy->SetActorTickEnabled(false);
    Enemy->SetActorLocation(FVector(0.f, 0.f, -10000.f));
}
