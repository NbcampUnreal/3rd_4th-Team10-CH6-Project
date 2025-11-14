// TTTGameModeBase.cpp


#include "GameSystem/GameMode/TTTGameModeBase.h"
#include "TimerManager.h"
#include "Enemy/System/SpawnSubsystem.h"
#include "Enemy/System/PoolSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameSystem/Player/TTTPlayerController.h"

ATTTGameModeBase::ATTTGameModeBase()
{
	GameStateClass = ATTTGameStateBase::StaticClass();
	PlayerControllerClass = ATTTPlayerController::StaticClass();
}

void ATTTGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SetupDataTables();
		StartPhase(ETTTGamePhase::Waiting, GetDefaultDurationFor(ETTTGamePhase::Waiting));
	}
}
void ATTTGameModeBase::SetupDataTables()
{

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UPoolSubsystem* PoolSystem = GI->GetSubsystem<UPoolSubsystem>())
		{
			PoolSystem->SetupEnemyTable(EnemyDataTableAsset);
		}
		if (USpawnSubsystem* SpawnSystem = GI->GetSubsystem<USpawnSubsystem>())
		{
			SpawnSystem->SetupWaveTable(WaveDataTableAsset);
		}
	}
}
void ATTTGameModeBase::StartPhase(ETTTGamePhase NewPhase, int32 DurationSeconds)
{
	if (!HasAuthority()) return;

	if (ATTTGameStateBase* S = GS())
	{
		S->Phase = NewPhase;
		S->OnRep_Phase();

		S->RemainingTime = DurationSeconds;
		S->OnRep_RemainingTime();

		GetWorldTimerManager().ClearTimer(TimerHandle_Tick1s);

		if (DurationSeconds > 0)
		{
			GetWorldTimerManager().SetTimer(
				TimerHandle_Tick1s, this, &ATTTGameModeBase::TickPhaseTimer, 1.0f, true
				);
		}
		else
		{
			AdvancePhase();
		}
	}
}

void ATTTGameModeBase::TickPhaseTimer()
{
	if (ATTTGameStateBase* S = GS())
	{
		S->RemainingTime = FMath::Max(0,S->RemainingTime - 1);
		S->OnRep_RemainingTime();

		if (S->RemainingTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_Tick1s);
			AdvancePhase();
		}
	}
}
int32 ATTTGameModeBase::GetDefaultDurationFor(ETTTGamePhase Phase) const
{
	switch (Phase)
	{
	case ETTTGamePhase::Waiting: return 5;
	case ETTTGamePhase::Build:   return 5;
	case ETTTGamePhase::Combat:  return 5;
	case ETTTGamePhase::Reward:  return 5;
	default:                     return 0; // Victory/GameOver
	}
}
void ATTTGameModeBase::EndGame(bool bVictory)
{
	if (ATTTGameStateBase* S = GS())
	{
		S->Phase = bVictory ? ETTTGamePhase::Victory : ETTTGamePhase::GameOver;
		S->OnRep_Phase();
		GetWorldTimerManager().ClearTimer(TimerHandle_Tick1s); // 더 이상 AdvancePhase 안 부름
	}
}

void ATTTGameModeBase::AdvancePhase()
{
	if (!HasAuthority()) return;

	if (ATTTGameStateBase* S = GS())
	{
		switch (S->Phase)
		{
		case ETTTGamePhase::Waiting:
			StartPhase(ETTTGamePhase::Build,  GetDefaultDurationFor(ETTTGamePhase::Build));
			break;

		case ETTTGamePhase::Build:
			StartPhase(ETTTGamePhase::Combat, GetDefaultDurationFor(ETTTGamePhase::Combat));
          
			if (UWorld* World = GetWorld())
			{
				if (USpawnSubsystem* SpawnSystem = GetGameInstance()->GetSubsystem<USpawnSubsystem>())
				{
					SpawnSystem->StartWave(S->Wave); 
				}
			}
			break; 

		case ETTTGamePhase::Combat:
			StartPhase(ETTTGamePhase::Reward, GetDefaultDurationFor(ETTTGamePhase::Reward));
			break;

		case ETTTGamePhase::Reward:
			S->Wave += 1;
			if (S->Wave >= MaxWaves) {EndGame(true);}
			else{StartPhase(ETTTGamePhase::Build,  GetDefaultDurationFor(ETTTGamePhase::Build));}
			break;

			
		default:
			// Victory/GameOver: 추가 진행 없음
			break;
		}
	}
}


void ATTTGameModeBase::PM_SetPhase(const FString& Name)
{
	if (!HasAuthority()) return;

	const FString L = Name.ToLower();

	if (L == TEXT("waiting")) StartPhase(ETTTGamePhase::Waiting, GetDefaultDurationFor(ETTTGamePhase::Waiting));
	else if (L == TEXT("build")) StartPhase(ETTTGamePhase::Build,  GetDefaultDurationFor(ETTTGamePhase::Build));
	else if (L == TEXT("combat")) StartPhase(ETTTGamePhase::Combat,  GetDefaultDurationFor(ETTTGamePhase::Combat));
	else if (L == TEXT("reward")) StartPhase(ETTTGamePhase::Reward,  GetDefaultDurationFor(ETTTGamePhase::Reward));
}
