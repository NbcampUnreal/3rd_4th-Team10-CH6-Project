// Fill out your copyright notice in the Description page of Project Settings.

#include "PreloadSubsystem.h"
#include "AbilitySystemGlobals.h"
#include "Enemy/Data/WaveData.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

// 데이터 테이블 설정 후 초기화
void UPreloadSubsystem::SetupTable(UDataTable* InWaveTable)
{
    if (!InWaveTable)
    {
        return;
    }
    WaveTable = InWaveTable;
    InitializeEnemies();
}
//Enemy 생성 및 초기화
void UPreloadSubsystem::InitializeEnemies()
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
        FEnemyWave& WaveData = PreloadedEnemies.FindOrAdd(WaveIndex);
        
        for (const FEnemySpawnInfo& EnemyInfo : Data->EnemyGroups)
        {
            if (!EnemyInfo.EnemyBP)
            {
                continue;
            }
            FEnemyArray& EnemyArray = WaveData.Waves.FindOrAdd(EnemyInfo.EnemyBP);
            TArray<AEnemyBase*>& EnemyList = EnemyArray.Enemies;

            int32 PreloadSize = EnemyInfo.bIsBoss ? BOSS_PRELOAD_SIZE : EnemyInfo.SpawnCount;
            for (int32 i = 0; i < PreloadSize; ++i)
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
                
                Enemy->SetActorHiddenInGame(true);
                Enemy->SetActorEnableCollision(false);
                Enemy->SetActorTickEnabled(false);
                EnemyList.Add(Enemy);
            }
            UE_LOG(LogTemp, Warning, TEXT("[Pool Init] Wave %d / %s  -> %d Created"),
              WaveIndex,
              *EnemyInfo.EnemyBP->GetName(),
              EnemyList.Num()
          );
        }
    }
}
//생성한 Enemy 가져오기
AEnemyBase* UPreloadSubsystem::GetEnemy(int32 WaveIndex, const FEnemySpawnInfo& EnemyInfo)
{
    FEnemyWave* WaveData = PreloadedEnemies.Find(WaveIndex);
    if (!WaveData)
    {
        return nullptr;
    }
    
    FEnemyArray* EnemyArray = WaveData->Waves.Find(EnemyInfo.EnemyBP);
    if (!EnemyArray || EnemyArray->Enemies.Num() == 0)
    {
        return nullptr;
    }
    
   // AEnemyBase* Enemy = EnemyArray->Pop();
    AEnemyBase* Enemy = EnemyArray->Enemies[0];
    EnemyArray->Enemies.RemoveAt(0);
    
    if (!Enemy)
    {
        return nullptr;
    }
    Enemy->InitializeEnemy();
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
/*void UPreloadSubsystem::ReleaseEnemy(int32 WaveIndex, AEnemyBase* Enemy)
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
void UPreloadSubsystem::DeactivateEnemy(AEnemyBase* Enemy)
{
    if (!Enemy)
    {
        return;
    }
    Enemy->ResetEnemy();
}*/