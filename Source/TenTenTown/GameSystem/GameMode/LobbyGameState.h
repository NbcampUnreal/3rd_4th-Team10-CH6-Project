// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALobbyGameState();

	/** 접속 인원 수 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
	int32 ConnectedPlayers;

	/** Ready 인원 수 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
	int32 ReadyPlayers;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
