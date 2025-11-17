// TTTGameModeBase.cpp


#include "GameSystem/GameMode/TTTGameModeBase.h"
#include "TimerManager.h"
#include "Enemy/System/SpawnSubsystem.h"
#include "Enemy/System/PoolSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"

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

	// 게임 시작 페이즈 초기화 (원래 쓰던 로직 있으면 그대로 유지)
	if (ATTTGameStateBase* GameStateBase = GS())
	{
		GameStateBase->Phase = ETTTGamePhase::Waiting;
	}

	// === 서버에서만 스폰 처리 ===
	if (!HasAuthority())
	{
		return;
	}

	// Debug: PlayerArray 안에 PlayerState + SelectedClass 찍어보기
	if (GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TTTGameModeBase::BeginPlay] PlayerArray dump"));
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS))
			{
				UE_LOG(LogTemp, Warning,
					TEXT("  PS=%s  SelectedClass=%s  Ready=%d"),
					*TTTPS->GetPlayerName(),
					*GetNameSafe(TTTPS->SelectedCharacterClass),
					TTTPS->IsReady());
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

void ATTTGameModeBase::RestartPlayer(AController* NewPlayer)
{
	// 죽었다가 부활 같은 상황용: 선택된 캐릭터 있으면 그대로 다시 스폰
	if (!HasAuthority() || !NewPlayer)
	{
		return;
	}

	APawn* NewPawn = SpawnSelectedCharacter(NewPlayer);

	// 선택된 캐릭터 정보가 없으면, 그냥 기본 동작으로 돌려보내도 됨
	if (!NewPawn)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[RestartPlayer] Fallback to Super::RestartPlayer for %s"),
			*GetNameSafe(NewPlayer));
		Super::RestartPlayer(NewPlayer);
	}
}

void ATTTGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (APlayerController* PC = Cast<APlayerController>(C))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[HandleSeamlessTravelPlayer] PC=%s PlayerState=%s"),
			*GetNameSafe(PC),
			*GetNameSafe(PC->PlayerState));
		SpawnSelectedCharacter(PC);
	}
}

APawn* ATTTGameModeBase::SpawnSelectedCharacter(AController* NewPlayer)
{
	if (!HasAuthority() || !NewPlayer)
	{
		return nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(NewPlayer);
	if (!PC)
	{
		return nullptr;
	}

	// 1) 플레이어 이름 가져오기
	FString PlayerName = TEXT("Unknown");
	if (APlayerState* PS_Base = PC->PlayerState)
	{
		PlayerName = PS_Base->GetPlayerName();
	}

	// 2) GameInstance에서 선택한 캐릭터 클래스 조회
	UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>();
	TSubclassOf<APawn> SelectedClass = nullptr;

	if (GI && !PlayerName.IsEmpty())
	{
		SelectedClass = GI->GetSelectedCharacter(PlayerName);
	}

	if (!SelectedClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SpawnSelectedCharacter] NO SelectedClass for PlayerName=%s"),
			*PlayerName);
		return nullptr;
	}

	// (선택) PlayerState에도 다시 써줌 (일관성 유지용)
	if (ATTTPlayerState* PS = PC->GetPlayerState<ATTTPlayerState>())
	{
		if (PS->SelectedCharacterClass != SelectedClass)
		{
			PS->SelectedCharacterClass = SelectedClass;
		}
	}

	// 3) 기존 Pawn이 있고, 같은 클래스면 유지
	if (APawn* ExistingPawn = PC->GetPawn())
	{
		if (ExistingPawn->GetClass() == SelectedClass)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[SpawnSelectedCharacter] PC=%s already has Pawn=%s (class ok)"),
				*GetNameSafe(PC),
				*GetNameSafe(ExistingPawn));
			return ExistingPawn;
		}

		ExistingPawn->Destroy();
	}

	// 4) 시작 위치 찾기
	AActor* StartSpot = FindPlayerStart(PC);
	FTransform SpawnTransform = StartSpot
		? StartSpot->GetActorTransform()
		: FTransform(FRotator::ZeroRotator, FVector::ZeroVector);

	FActorSpawnParameters Params;
	Params.Owner = PC;
	Params.Instigator = PC->GetPawn();
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 5) 스폰 + 포제션
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
		SelectedClass,
		SpawnTransform,
		Params);

	if (!NewPawn)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[SpawnSelectedCharacter] SpawnActor FAILED PC=%s Class=%s"),
			*GetNameSafe(PC),
			*GetNameSafe(SelectedClass));
		return nullptr;
	}

	PC->Possess(NewPawn);

	UE_LOG(LogTemp, Warning,
		TEXT("[SpawnSelectedCharacter] Spawned Pawn=%s for PC=%s using Class=%s (PlayerName=%s)"),
		*GetNameSafe(NewPawn),
		*GetNameSafe(PC),
		*GetNameSafe(SelectedClass),
		*PlayerName);

	return NewPawn;
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