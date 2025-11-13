// TTTGameModeBase.cpp


#include "GameSystem/GameMode/TTTGameModeBase.h"
#include "TimerManager.h"
#include "Enemy/System/SpawnSubsystem.h"
#include "Enemy/System/PoolSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Character/PS/TTTPlayerState.h"

ATTTGameModeBase::ATTTGameModeBase()
{
	GameStateClass = ATTTGameStateBase::StaticClass();
	PlayerControllerClass = ATTTPlayerController::StaticClass();

	PlayerStateClass      = ATTTPlayerState::StaticClass();
	bUseSeamlessTravel    = true;
	bStartPlayersAsSpectators = true;  

}

void ATTTGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (APawn* OldPawn = PC->GetPawn())
				{
					OldPawn->Destroy();
				}

				RestartPlayer(PC);
			}
		}
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

void ATTTGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	RestartPlayer(NewPlayer);
}

void ATTTGameModeBase::RestartPlayer(AController* NewPlayer)
{
	if (!NewPlayer)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(NewPlayer);
	if (!PC)
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	// 1) PlayerState에서 선택된 캐릭터 클래스 가져오기
	ATTTPlayerState* PS = PC->GetPlayerState<ATTTPlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameModeBase::RestartPlayer] No TTTPlayerState"));
		return;
	}

	TSubclassOf<APawn> SpawnClass = PS->SelectedCharacterClass;
	if (!SpawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameModeBase::RestartPlayer] SelectedCharacterClass is null"));
		return;
	}

	// 2) 기존 Pawn 정리
	if (APawn* OldPawn = PC->GetPawn())
	{
		OldPawn->Destroy();
	}

	// 3) PlayerStart 위치 찾기
	AActor* StartSpot = FindPlayerStart(PC);
	const FVector SpawnLoc = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;
	const FRotator SpawnRot = StartSpot ? StartSpot->GetActorRotation() : FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.Owner = PC;

	// 4) Pawn 스폰 + Possess
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(SpawnClass, SpawnLoc, SpawnRot, Params);
	if (NewPawn)
	{
		PC->Possess(NewPawn);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[GameModeBase::RestartPlayer] Failed to spawn pawn for player"));
	}
}

void ATTTGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (APlayerController* PC = Cast<APlayerController>(C))
	{
		// 여기서도 OldPawn 정리 + 선택된 캐릭터 스폰
		if (APawn* OldPawn = PC->GetPawn())
		{
			OldPawn->Destroy();
		}

		RestartPlayer(PC);
	}
}

void ATTTGameModeBase::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (PC->PlayerState)
			{
				ActorList.Add(PC->PlayerState);  // ★ 유지!
			}
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
