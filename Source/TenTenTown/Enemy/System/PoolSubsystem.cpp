// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolSubsystem.h"

#include "AbilitySystemGlobals.h"
#include "Enemy/Data/EnemyData.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"

// 데이터 테이블 설정 후 초기화
void UPoolSubsystem::SetupEnemyTable(UDataTable* InEnemyTable)
{
    if (!InEnemyTable) return;
    EnemyTable = InEnemyTable;
    InitializePool();
}

//풀 초기화
void UPoolSubsystem::InitializePool()
{
    if (!EnemyTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("PoolSubsystem: EnemyTable is null"));
        return;
    }

    if (!GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("PoolSubsystem: World is null"));
        return;
    }

    TArray<FName> RowNames = EnemyTable->GetRowNames();
    UE_LOG(LogTemp, Log, TEXT("PoolSubsystem: Found %d rows in EnemyTable"), RowNames.Num());

    for (FName RowName : RowNames)
    {
        const FEnemyData* Data = EnemyTable->FindRow<FEnemyData>(RowName, TEXT("InitializePool"));
        if (!Data)
        {
            UE_LOG(LogTemp, Warning, TEXT("PoolSubsystem: Failed to find row %s"), *RowName.ToString());
            continue;
        }

        if (!Data->EnemyBP)
        {
            UE_LOG(LogTemp, Warning, TEXT("PoolSubsystem: EnemyBP is null for %s"), *Data->EnemyName.ToString());
            continue;
        }

        TArray<AEnemyBase*>& Pool = EnemyPools.FindOrAdd(Data->EnemyName);
        if (Pool.Num() > 0)
        {
            UE_LOG(LogTemp, Log, TEXT("PoolSubsystem: Pool already exists for %s"), *Data->EnemyName.ToString());
            continue;
        }

        UE_LOG(LogTemp, Log, TEXT("PoolSubsystem: Initializing pool for %s"), *Data->EnemyName.ToString());

        for (int32 i = 0; i < INITIAL_POOL_SIZE; ++i)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            FVector SpawnLocation(0.f, 0.f, -10000.f);
            FRotator SpawnRotation = FRotator::ZeroRotator;

            AEnemyBase* Enemy = GetWorld()->SpawnActor<AEnemyBase>(Data->EnemyBP, SpawnLocation, SpawnRotation, SpawnParams);
            if (!Enemy)
            {
                UE_LOG(LogTemp, Warning, TEXT("PoolSubsystem: Failed to spawn Enemy for %s"), *Data->EnemyName.ToString());
                continue;
            }

            Enemy->SpawnDefaultController();

            UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Enemy);
            
            //AttributeSet 세팅
            if (const UAS_EnemyAttributeSetBase* Attr = Enemy->GetAttributeSet())
            {
                //ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMaxHealthAttribute(), Data->MaxHP);
                //ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetHealthAttribute(), Data->MaxHP);
                //ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute(), Data->Attack);
                //ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetMovementSpeedAttribute(), Data->MovementSpeed);
                //ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackRangeAttribute(), Data->AttackRange);
                //ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetGoldAttribute(), Data->Gold);
                //ASC->SetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetExpAttribute(), Data->EXP);
                //Attr->SetMaxHealth(Data->MaxHP);
                //Attr->SetHealth(Data->CurrentHP);
                //Attr->SetAttack(Data->Attack);
                //Attr->SetMovementSpeed(Data->MovementSpeed);
                //Attr->SetAttackSpeed(Data->AttackSpeed);
                //Attr->SetAttackRange(Data->AttackRange);
                //Attr->SetGold(Data->Gold);

            }

     
            DeactivateEnemy(Enemy);
            Pool.Add(Enemy);

            UE_LOG(LogTemp, Log, TEXT("PoolSubsystem: Spawned and added Enemy %s (%d/%d) to pool"), *Data->EnemyName.ToString(), i+1, INITIAL_POOL_SIZE);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("PoolSubsystem: Pool initialization complete"));
}


// 풀에서 Enemy 획득
AEnemyBase* UPoolSubsystem::GetPooledEnemy(FName EnemyName)
{
    TArray<AEnemyBase*>* Pool = EnemyPools.Find(EnemyName);
    if (!Pool || Pool->Num() == 0)
    {
        return nullptr;
    }

    AEnemyBase* Enemy = Pool->Pop();
    if (!Enemy)
    {
        return nullptr;
    }
    
    return Enemy;
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
        if (Data && Data->EnemyBP.Get() == Enemy->GetClass())
        {
            PoolKey = Data->EnemyName;
            break;
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
        Controller->UnPossess();
    }

    Enemy->SetActorHiddenInGame(true);
    Enemy->SetActorEnableCollision(false);
    Enemy->SetActorTickEnabled(false);
    Enemy->SetActorLocation(FVector(0.f, 0.f, -10000.f));
}
