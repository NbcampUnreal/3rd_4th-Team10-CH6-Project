// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/GameMode/LobbyGameMode.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Character/PS/TTTPlayerState.h"

#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass        = ALobbyGameState::StaticClass();
	PlayerControllerClass = ATTTPlayerController::StaticClass();
	PlayerStateClass      = ATTTPlayerState::StaticClass();

	MinPlayersToStart = 2; // <- 여기서 2인용 테스트
	InGameMapPath = TEXT("/Game/Maps/InGameMap"); // 실제 인게임 맵 경로로 수정
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UpdateLobbyCounts();
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UpdateLobbyCounts();
}

void ALobbyGameMode::HandlePlayerReadyChanged(ATTTPlayerState* ChangedPlayerState)
{
	UpdateLobbyCounts();
	TryStartGame();
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

void ALobbyGameMode::TryStartGame()
{
	ALobbyGameState* LobbyGS = GetGameState<ALobbyGameState>();
	if (!LobbyGS)
	{
		return;
	}

	// 인원 부족
	if (LobbyGS->ConnectedPlayers < MinPlayersToStart)
	{
		return;
	}

	// 아직 모두 준비 X
	if (LobbyGS->ReadyPlayers != LobbyGS->ConnectedPlayers)
	{
		return;
	}

	// 모든 인원이 캐릭터 선택 + Ready 완료 → 인게임으로 이동
	if (UWorld* World = GetWorld())
	{
		if (InGameMapPath.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("[LobbyGameMode] InGameMapPath is empty!"));
			return;
		}

		const FString URL = InGameMapPath + TEXT("?listen");
		UE_LOG(LogTemp, Log, TEXT("[LobbyGameMode] ServerTravel -> %s"), *URL);

		World->ServerTravel(URL, false);
	}
}