// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolSubsystem.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Data/WaveData.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/Engine.h"
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
    for (int32 WaveIndex = 0; WaveIndex < RowNames.Num(); ++WaveIndex)
    {
        const FWaveData* Data = WaveTable->FindRow<FWaveData>(RowNames[WaveIndex], TEXT("InitializePool"));
        if (!Data)
        {
            continue;
        }
        TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>>& WavePool = EnemyPools.FindOrAdd(WaveIndex);

        for (const FEnemySpawnInfo& EnemyInfo : Data->EnemyGroups)
        {
            if (!EnemyInfo.EnemyBP)
            {
                continue;
            }
            TArray<AEnemyBase*>& Pool = WavePool.FindOrAdd(EnemyInfo.EnemyBP);

            int32 PoolSize = EnemyInfo.bIsBoss ? BOSS_POOL_SIZE : EnemyInfo.SpawnCount;
            for (int32 i = 0; i < PoolSize; ++i)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AEnemyBase* Enemy = GetWorld()->SpawnActor<AEnemyBase>(
                    EnemyInfo.EnemyBP,
                    FVector(0.f, 0.f, -10000.f),
                    FRotator::ZeroRotator,
                    SpawnParams
                );

                if (!Enemy)
                {
                    continue;
                }
                Enemy->SpawnWaveIndex = WaveIndex; 
                DeactivateEnemy(Enemy);
                Pool.Add(Enemy);
            }
            UE_LOG(LogTemp, Warning, TEXT("[Pool Init] Wave %d / %s  -> %d Created"),
              WaveIndex,
              *EnemyInfo.EnemyBP->GetName(),
              Pool.Num()
          );
        }
    }
}
//풀에서 Enemy 획득
AEnemyBase* UPoolSubsystem::GetPooledEnemy(int32 WaveIndex, const FEnemySpawnInfo& EnemyInfo)
{
    TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>>* WavePool = EnemyPools.Find(WaveIndex);
    TArray<AEnemyBase*>* Pool = WavePool->Find(EnemyInfo.EnemyBP);
    if (!Pool || Pool->Num() == 0)
    {
        return nullptr;
    }
    
   // AEnemyBase* Enemy = Pool->Pop();
    AEnemyBase* Enemy = Pool->GetData()[0];
    Pool->RemoveAt(0);
    
    Enemy->InitializeEnemy();

    if (!Enemy)
    {
        return nullptr;
    }
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Enemy))
    {
        if (const UAS_EnemyAttributeSetBase* AttrSet = ASC->GetSet<UAS_EnemyAttributeSetBase>())
        {
            float Mult = EnemyInfo.StatMultiplier;
            float NewMaxHealth = AttrSet->GetMaxHealth() * Mult;
            float NewAttack = AttrSet->GetAttack() * Mult;
            ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute(), NewMaxHealth);
            ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetHealthAttribute(), NewMaxHealth);
            ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute(), NewAttack);

        }
    }

    return Enemy;
}

// 사용된 Enemy 풀에 반환
void UPoolSubsystem::ReleaseEnemy(int32 WaveIndex, AEnemyBase* Enemy)
{
    if (!Enemy)
    {
        return;
    }
    
    DeactivateEnemy(Enemy);

    if (!WaveTable)
    {
        return;
    }
    TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>>* WavePool = EnemyPools.Find(WaveIndex);
    if (!WavePool)
    {
        return;
    }
    TSubclassOf<AEnemyBase> PoolKey;

    const FWaveData* Data = WaveTable->FindRow<FWaveData>(WaveTable->GetRowNames()[WaveIndex], TEXT(""));
    if (!Data)
    {
        return;
    }
    for (const FEnemySpawnInfo& EnemyInfo : Data->EnemyGroups)
    {
        if (Enemy->IsA(EnemyInfo.EnemyBP))
        {
            PoolKey = EnemyInfo.EnemyBP;
            break;
        }
    }

    WavePool->FindOrAdd(PoolKey).Add(Enemy);
}

// Enemy 비활성화
void UPoolSubsystem::DeactivateEnemy(AEnemyBase* Enemy)
{
    if (!Enemy)
    {
        return;
    }
    Enemy->ResetEnemy();
}