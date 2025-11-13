// LobbyGameMode.cpp


#include "GameSystem/GameMode/LobbyGameMode.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Character/PS/TTTPlayerState.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass        = ALobbyGameState::StaticClass();
	PlayerControllerClass = ATTTPlayerController::StaticClass();
	PlayerStateClass      = ATTTPlayerState::StaticClass();

	MinPlayersToStart = 2; // <- 여기서 2인용 테스트
	InGameMapPath = TEXT("/Game/Maps/InGameMap"); // 실제 인게임 맵 경로로 수정

	CountdownStartValue = 5;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UpdateLobbyCounts();
	if (GetGameState<ALobbyGameState>())
	{
		ALobbyGameState* GS = GetGameState<ALobbyGameState>();
		GS->ConnectedPlayers = GS->PlayerArray.Num();
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UpdateLobbyCounts();
	if (ALobbyGameState* GS = GetGameState<ALobbyGameState>())
	{
		GS->ConnectedPlayers = GS->PlayerArray.Num();
	}
}

void ALobbyGameMode::HandlePlayerReadyChanged(ATTTPlayerState* ChangedPlayerState)
{
	UpdateLobbyCounts();
	CheckAllReady();
}

void ALobbyGameMode::UpdateLobbyCounts()
{
	ALobbyGameState* LobbyGS = GetGameState<ALobbyGameState>();
	if (!LobbyGS || !GameState)
	{
		return;
	}

	int32 Total = 0;
	int32 Ready = 0;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS))
		{
			++Total;

			// 선택된 캐릭터가 있고, Ready면 카운트
			if (TTTPS->SelectedCharacterClass && TTTPS->IsReady())
			{
				++Ready;
			}
		}
	}

	LobbyGS->ConnectedPlayers = Total;
	LobbyGS->ReadyPlayers     = Ready;
}

void ALobbyGameMode::CheckAllReady()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return;

	// 이미 Loading/인게임이면 다시 시작하지 않음
	if (GS->LobbyPhase != ELobbyPhase::Waiting)
	{
		return;
	}

	// 모든 플레이어가 Ready && 최소 인원 이상
	if (GS->ReadyPlayers == GS->ConnectedPlayers &&
		GS->ConnectedPlayers >= MinPlayersToStart)
	{
		// 1) Phase를 Loading으로 전환
		GS->LobbyPhase = ELobbyPhase::Loading;
		GS->OnRep_LobbyPhase();

		// 2) 카운트다운 값 세팅 (5초)
		GS->CountdownSeconds = CountdownStartValue;
		GS->OnRep_CountdownSeconds();

		// 3) 1초 간격으로 TickCountdown() 호출
		GetWorld()->GetTimerManager().SetTimer(
			StartCountdownTimerHandle,
			this,
			&ALobbyGameMode::TickCountdown,
			1.0f,
			true  // 반복
		);
	}
}

void ALobbyGameMode::StartGameTravel()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (GS)
	{
		GS->CountdownSeconds = 0;
		GS->OnRep_CountdownSeconds();
	}

	UWorld* World = GetWorld();
	if (!World) return;

	const FString Url = FString::Printf(TEXT("%s?listen"), *InGameMapPath);
	World->ServerTravel(Url, /*bAbsolute*/ false);
}
void ALobbyGameMode::TickCountdown()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS)
	{
		return;
	}

	// 안전장치: 로비 Phase가 아니면 카운트다운 중지
	if (GS->LobbyPhase != ELobbyPhase::Loading)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartCountdownTimerHandle);
		return;
	}

	// 초 줄이기
	if (GS->CountdownSeconds > 0)
	{
		GS->CountdownSeconds--;
		GS->OnRep_CountdownSeconds();
	}

	// 0이 되면 타이머 정지 후 인게임으로 이동
	if (GS->CountdownSeconds <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartCountdownTimerHandle);
		StartGameTravel();
	}
}