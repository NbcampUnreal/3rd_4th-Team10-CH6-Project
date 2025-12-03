// CharacterLootSubsystem.cpp
#include "CharacterLootSubsystem.h"
#include "Engine/Engine.h"
#include "GameSystem/GameMode/TTTGameStateBase.h" 

void UCharacterLootSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, TEXT("LootSubsystem Initialized"));
}

void UCharacterLootSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

ATTTGameStateBase* UCharacterLootSubsystem::GetGameState()
{
    return GetWorld() ? GetWorld()->GetGameState<ATTTGameStateBase>() : nullptr;
}

bool UCharacterLootSubsystem::IsServerSide()
{
    // 클라이언트(3)보다 작으면 서버(0,1,2)
    return GetWorld() && (GetWorld()->GetNetMode() < NM_Client);
}


int32 UCharacterLootSubsystem::GetGold()
{
    if (ATTTGameStateBase* GS = GetGameState())
    {
        return GS->Gold;
    }
    return 0;
}

float UCharacterLootSubsystem::GetEXP()
{
    if (ATTTGameStateBase* GS = GetGameState())
    {
        return GS->EXP;
    }
    return 0.0f;
}

void UCharacterLootSubsystem::SetGold(int32 NewGold)
{
    if (!IsServerSide()) return;
    
    if (ATTTGameStateBase* GS = GetGameState())
    {
        GS->Gold = NewGold;
    }
}

void UCharacterLootSubsystem::SetEXP(float NewEXP)
{
    if (!IsServerSide()) return;

    if (ATTTGameStateBase* GS = GetGameState())
    {
        GS->EXP = NewEXP;
    }
}

void UCharacterLootSubsystem::AddGold(int32 PlusGold)
{
    if (!IsServerSide()) return;

    if (ATTTGameStateBase* GS = GetGameState())
    {
        GS->Gold += PlusGold;
    }
}

void UCharacterLootSubsystem::AddEXP(float PlusEXP)
{
    if (!IsServerSide()) return;

    if (ATTTGameStateBase* GS = GetGameState())
    {
        GS->EXP += PlusEXP;
    }
}

void UCharacterLootSubsystem::SetZero()
{
    if (!IsServerSide()) return;

    if (ATTTGameStateBase* GS = GetGameState())
    {
        GS->Gold = 0;
        GS->EXP = 0.0f;
    }
}