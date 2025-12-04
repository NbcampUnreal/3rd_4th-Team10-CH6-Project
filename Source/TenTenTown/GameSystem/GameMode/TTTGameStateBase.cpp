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
    const UEnum* EnumPtr = StaticEnum<ETTTGamePhase>();
    const FString PhaseName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)Phase) : TEXT("Unknown");

    UE_LOG(LogTemp, Warning, TEXT("[GameState] Phase Changed -> %s (%d)"), *PhaseName, (int32)Phase);

    OnPhaseChanged.Broadcast(Phase);
}

void ATTTGameStateBase::OnRep_RemainingTime()
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);
}
void ATTTGameStateBase::OnRep_RemainEnemy()
{
    // C++ delegate
    OnRemainEnemyChangedDelegate.Broadcast(RemainEnemy);

    // BP delegate
    OnRemainEnemyChanged.Broadcast(RemainEnemy);
}
void ATTTGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTTGameStateBase, Phase);
	DOREPLIFETIME(ATTTGameStateBase, RemainingTime);
	DOREPLIFETIME(ATTTGameStateBase, Wave);
    DOREPLIFETIME(ThisClass,Gold);
    DOREPLIFETIME(ThisClass,EXP);

    DOREPLIFETIME(ATTTGameStateBase, CoreHealth);
    DOREPLIFETIME(ATTTGameStateBase, WaveLevel);
    DOREPLIFETIME(ATTTGameStateBase, RemainEnemy);
}


#pragma region UI_Region
void ATTTGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);

    // �� PlayerState�� TTTPlayerState Ÿ������ Ȯ��
    if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PlayerState))
    {
        // ��������Ʈ ȣ��: Manager ViewModel���� �� ����� ������ �˸��ϴ�.
        //OnPlayerJoinedDelegate.Broadcast(TTTPlayerState);
    }
}

void ATTTGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
    // ������ PlayerState�� TTTPlayerState Ÿ������ Ȯ��
    if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PlayerState))
    {
        // ��������Ʈ ȣ��: Manager ViewModel���� ����� �������� �˸��ϴ�.
        //OnPlayerLeftDelegate.Broadcast(TTTPlayerState);
    }

    Super::RemovePlayerState(PlayerState);
}

//TArray<ATTTPlayerState*> ATTTGameStateBase::GetAllCurrentPartyMembers() const
//{
//    TArray<ATTTPlayerState*> PartyMembers;
//    for (APlayerState* PS : PlayerArray)
//    {
//        if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PS))
//        {
//            // ���� ��Ƽ �ý��ۿ����� ���⿡ '�� �÷��̾ ���� ��Ƽ�� ���� �ִ���' Ȯ���ϴ� ������ ���ϴ�.
//            // ����� ��� ���� �÷��̾ ��Ƽ������ �����ϰ� ��ȯ�մϴ�.
//            PartyMembers.Add(TTTPlayerState);
//        }
//    }
//    return PartyMembers;
//}

//TArray<ATTTPlayerState*> ATTTGameStateBase::GetAllCurrentPartyMembers(ATTTPlayerState* ExcludePlayerState) const
//{
//    TArray<ATTTPlayerState*> PartyMembers;
//	//PlayerArray�� �� �α� ���
//	UE_LOG(LogTemp, Log, TEXT("GetAllCurrentPartyMembers: PlayerArray Num = %d"), PlayerArray.Num());
//	//ExcludePlayerState �α� ���
//	UE_LOG(LogTemp, Log, TEXT("GetAllCurrentPartyMembers: ExcludePlayerState = %s"), ExcludePlayerState ? *ExcludePlayerState->GetPlayerName() : TEXT("NULL"));
//
//    const int32 ExcludePlayerId = ExcludePlayerState ? ExcludePlayerState->GetPlayerId() : INDEX_NONE;
//
//    for (APlayerState* PS : PlayerArray)
//    {
//        if (ATTTPlayerState* TTTPlayerState = Cast<ATTTPlayerState>(PS))
//        {
//            // ���� ��Ƽ �ý��ۿ����� ���⿡ '�� �÷��̾ ���� ��Ƽ�� ���� �ִ���' Ȯ���ϴ� ������ ���ϴ�.
//            // ����� ��� ���� �÷��̾ ��Ƽ������ �����ϰ� ��ȯ�մϴ�.
//            if (TTTPlayerState->GetPlayerId() != ExcludePlayerId)
//            {
//                PartyMembers.Add(TTTPlayerState);
//            }
//        }
//    }
//    return PartyMembers;
//}

//2. �÷��̾� ���� �� ��������Ʈ ȣ�� (���� ����)
void ATTTGameStateBase::NotifyPlayerJoined(ATTTPlayerState* NewPlayerState)
{
    // ��Ƽĳ��Ʈ ��������Ʈ ȣ�� -> UPartyManagerViewModel::HandlePlayerJoined ����
    //OnPlayerJoinedDelegate.Broadcast(NewPlayerState);
}

//3. �÷��̾� ���� ���� �� ��������Ʈ ȣ�� (���� ����)
void ATTTGameStateBase::NotifyPlayerLeft(ATTTPlayerState* LeavingPlayerState)
{
    // ��Ƽĳ��Ʈ ��������Ʈ ȣ�� -> UPartyManagerViewModel::HandlePlayerLeft ����
    //OnPlayerLeftDelegate.Broadcast(LeavingPlayerState);
}


void ATTTGameStateBase::SetCoreHealth(int32 NewCoreHealth)
{
    if (CoreHealth != NewCoreHealth)
    {
        CoreHealth = NewCoreHealth;

        // ��������Ʈ ȣ��: ���� ���� ViewModel�� ���� ������ ��� �˸��ϴ�.
        OnCoreHealthChangedDelegate.Broadcast(CoreHealth);

        // �������� ������ ����� ��� ���� ó���� �ʿ��մϴ�.
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
        OnRemainEnemyChanged.Broadcast(RemainEnemy); 
    }
}

void ATTTGameStateBase::NotifyPlayerReady()
{
    if (!HasAuthority())
    {
        return;
    }

    Multicast_PlayerJoinedBroadcast();
}
void ATTTGameStateBase::Multicast_PlayerJoinedBroadcast_Implementation()
{
    OnPlayerJoinedDelegate.Broadcast();
}
#pragma endregion

