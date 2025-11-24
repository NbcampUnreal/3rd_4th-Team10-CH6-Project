// TTTGameStateBase.cpp


#include "GameSystem/GameMode/TTTGameStateBase.h"

#include "Net/UnrealNetwork.h"
#include "Character/PS/TTTPlayerState.h"

ATTTGameStateBase::ATTTGameStateBase()
{
	bReplicates = true;
	Wave=0;
}

void ATTTGameStateBase::OnRep_Phase()
{
	OnPhaseChanged.Broadcast(Phase);
}

void ATTTGameStateBase::OnRep_RemainingTime()
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);
}

void ATTTGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTTGameStateBase, Phase);
	DOREPLIFETIME(ATTTGameStateBase, RemainingTime);
	DOREPLIFETIME(ATTTGameStateBase, Wave);
}


#pragma region UI_Region
void ATTTGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);

    // 새 PlayerState가 TTTPlayerState 타입인지 확인
    if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PlayerState))
    {
        // 델리게이트 호출: Manager ViewModel에게 새 멤버가 왔음을 알립니다.
        OnPlayerJoinedDelegate.Broadcast(TTTPlayerState);
    }
}

void ATTTGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
    // 나가는 PlayerState가 TTTPlayerState 타입인지 확인
    if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PlayerState))
    {
        // 델리게이트 호출: Manager ViewModel에게 멤버가 떠났음을 알립니다.
        OnPlayerLeftDelegate.Broadcast(TTTPlayerState);
    }

    Super::RemovePlayerState(PlayerState);
}

TArray<ATTTPlayerState*> ATTTGameStateBase::GetAllCurrentPartyMembers() const
{
    TArray<ATTTPlayerState*> PartyMembers;
    for (APlayerState* PS : PlayerArray)
    {
        if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PS))
        {
            // 실제 파티 시스템에서는 여기에 '이 플레이어가 현재 파티에 속해 있는지' 확인하는 로직이 들어갑니다.
            // 현재는 모든 접속 플레이어를 파티원으로 간주하고 반환합니다.
            PartyMembers.Add(TTTPlayerState);
        }
    }
    return PartyMembers;
}

//2. 플레이어 접속 시 델리게이트 호출 (서버 전용)
void ATTTGameStateBase::NotifyPlayerJoined(ATTTPlayerState* NewPlayerState)
{
    // 멀티캐스트 델리게이트 호출 -> UPartyManagerViewModel::HandlePlayerJoined 실행
    OnPlayerJoinedDelegate.Broadcast(NewPlayerState);
}

//3. 플레이어 접속 해제 시 델리게이트 호출 (서버 전용)
void ATTTGameStateBase::NotifyPlayerLeft(ATTTPlayerState* LeavingPlayerState)
{
    // 멀티캐스트 델리게이트 호출 -> UPartyManagerViewModel::HandlePlayerLeft 실행
    OnPlayerLeftDelegate.Broadcast(LeavingPlayerState);
}


void ATTTGameStateBase::SetCoreHealth(int32 NewCoreHealth)
{
    if (CoreHealth != NewCoreHealth)
    {
        CoreHealth = NewCoreHealth;

        // 델리게이트 호출: 구독 중인 ViewModel에 변경 사항을 즉시 알립니다.
        OnCoreHealthChangedDelegate.Broadcast(CoreHealth);

        // 서버에서 변수가 변경될 경우 복제 처리가 필요합니다.
        // if (HasAuthority()) { MarkDirtyReplication(); }
    }
}

void ATTTGameStateBase::SetRemainingTime(int32 NewRemainingTime)
{
    if (RemainingTime != NewRemainingTime)
    {
        RemainingTime = NewRemainingTime;
        OnRemainingTimeChangedDelegate.Broadcast(RemainingTime);
    }
}

void ATTTGameStateBase::SetWaveLevel(int32 NewWaveLevel)
{
    if (WaveLevel != NewWaveLevel)
    {
        WaveLevel = NewWaveLevel;
        OnWaveLevelChangedDelegate.Broadcast(WaveLevel);
    }
}

void ATTTGameStateBase::SetRemainEnemy(int32 NewRemainEnemy)
{
    if (RemainEnemy != NewRemainEnemy)
    {
        RemainEnemy = NewRemainEnemy;
        OnRemainEnemyChangedDelegate.Broadcast(RemainEnemy);
    }
}
#pragma endregion

