// LobbyGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

UENUM(BlueprintType)
enum class ELobbyPhase : uint8
{
	Waiting   UMETA(DisplayName = "Waiting"),
	ReadyCheck UMETA(DisplayName = "ReadyCheck"),
	Loading   UMETA(DisplayName = "Loading")
};

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

	// 로비 페이즈 (Replicate)
	UPROPERTY(ReplicatedUsing=OnRep_LobbyPhase, BlueprintReadOnly)
	ELobbyPhase LobbyPhase;

	UPROPERTY(ReplicatedUsing=OnRep_CountdownSeconds, BlueprintReadOnly, Category="Lobby")
	int32 CountdownSeconds;

	UFUNCTION()
	void OnRep_ReadyPlayers();

	UFUNCTION()
	void OnRep_ConnectedPlayers();

	UFUNCTION()
	void OnRep_LobbyPhase();

	UFUNCTION()
	void OnRep_CountdownSeconds();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
