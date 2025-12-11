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
	SelectedMapIndex = INDEX_NONE;
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
	DOREPLIFETIME(ALobbyGameState, SelectedMapIndex);
	DOREPLIFETIME(ALobbyGameState, PlayerResults);
}

void ALobbyGameState::SetConnectedPlayers(int32 NewCount)
{
    if (ConnectedPlayers != NewCount)
    {
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGameState] SetConnectedPlayers: %d -> %d"), ConnectedPlayers, NewCount);
        ConnectedPlayers = NewCount;

        OnPlayerCountChanged.Broadcast();
    }
}

void ALobbyGameState::SetReadyPlayers(int32 NewCount)
{
    if (ReadyPlayers != NewCount)
    {
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGameState] SetReadyPlayers: %d -> %d"), ReadyPlayers, NewCount);
        ReadyPlayers = NewCount;

        OnPlayerCountChanged.Broadcast();
    }
}

void ALobbyGameState::OnRep_SelectedMapIndex()
{
	OnSelectedMapChanged.Broadcast(SelectedMapIndex);
}

void ALobbyGameState::SetSelectedMapIndex(int32 NewIndex)
{
	if (HasAuthority() && SelectedMapIndex != NewIndex)
	{
		SelectedMapIndex = NewIndex;
		OnSelectedMapChanged.Broadcast(SelectedMapIndex);
	}
}

void ALobbyGameState::OnRep_PlayerResults()
{
	// 1. (디버깅용) 복제 성공 확인 로그 추가
	UE_LOG(LogTemp, Log, TEXT("ALobbyGameState: OnRep_PlayerResults called. Num: %d"), PlayerResults.Num());

	// 2. ViewModel 또는 UI에 데이터가 도착했음을 알리는 로직을 여기서 호출
	// 예: UI 관련 델리게이트 브로드캐스트 또는 ViewModel 업데이트 함수 호출

	// FOnPlayerResultReady 델리게이트 등을 사용하면 ViewModel이 이 시점에 UI를 열거나 업데이트할 수 있습니다.
	// OnPlayerResultReady.Broadcast(); 
}