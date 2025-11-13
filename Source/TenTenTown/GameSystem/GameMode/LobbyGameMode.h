// LobbyGameMode.h
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

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/** PlayerState에서 Ready가 바뀌면 호출 */
	void HandlePlayerReadyChanged(ATTTPlayerState* ChangedPlayerState);

protected:
	
	void UpdateLobbyCounts();

	UFUNCTION()
	void CheckAllReady();

	void TickCountdown();

	void StartGameTravel();

	/** 최소 시작 인원 (지금은 2명으로 테스트) */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	int32 MinPlayersToStart;

	/** 인게임 맵 경로 (예: "/Game/Maps/MainGame") */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	FString InGameMapPath;

	/** 카운트다운 시작 값(초) – 기본 5초 */
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	int32 CountdownStartValue = 5;

	/** 카운트다운용 타이머 핸들 */
	FTimerHandle StartCountdownTimerHandle;
	
	
};
