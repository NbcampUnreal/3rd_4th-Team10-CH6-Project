// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
class ATTTLobbyGameState;
class ATTTPlayerState;

UCLASS()
class TENTENTOWN_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ALobbyGameMode();

	/** PlayerState에서 Ready가 바뀌면 호출 */
	void HandlePlayerReadyChanged(ATTTPlayerState* ChangedPlayerState);

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void UpdateLobbyCounts();
	void TryStartGame();

	/** 최소 시작 인원 (지금은 2명으로 테스트) */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	int32 MinPlayersToStart;

	/** 인게임 맵 경로 (예: "/Game/Maps/MainGame") */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	FString InGameMapPath;
	
};
