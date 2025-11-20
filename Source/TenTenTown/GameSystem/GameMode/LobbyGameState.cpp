// LobbyGameState.cpp


#include "GameSystem/GameMode/LobbyGameState.h"
#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{
	bReplicates = true;
	ConnectedPlayers = 0;
	ReadyPlayers = 0;
	LobbyPhase = ELobbyPhase::Waiting;
	CountdownSeconds  = 0;
}

void ALobbyGameState::OnRep_ReadyPlayers()
{
	OnPlayerCountChanged.Broadcast();
}

void ALobbyGameState::OnRep_ConnectedPlayers()
{
	OnPlayerCountChanged.Broadcast();
}

void ALobbyGameState::OnRep_LobbyPhase()
{
}

void ALobbyGameState::OnRep_CountdownSeconds()
{
	OnCountdownChanged.Broadcast(CountdownSeconds);
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, ConnectedPlayers);
	DOREPLIFETIME(ALobbyGameState, ReadyPlayers);
	DOREPLIFETIME(ALobbyGameState, LobbyPhase);
	DOREPLIFETIME(ALobbyGameState, CountdownSeconds);
}
