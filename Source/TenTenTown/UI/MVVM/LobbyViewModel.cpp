#include "UI/MVVM/LobbyViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "FieldNotification/IFieldValueChanged.h"
#include "TenTenTown/GameSystem/Player/TTTPlayerController.h"


void ULobbyViewModel::Initialize(ALobbyGameState* InGS, ATTTPlayerController* InPC)
{
    CachedGameState = InGS;
    CachedPlayerController = InPC;
    InitializeViewModel();
}

void ULobbyViewModel::InitializeViewModel()
{
    // 부모 클래스의 초기화 호출
    Super::InitializeViewModel();

    if (CachedGameState)
    {
        HandlePlayerCountChanged();
        HandleCountdownChanged(CachedGameState->CountdownSeconds);

        CachedGameState->OnCountdownChanged.AddDynamic(
            this, &ULobbyViewModel::HandleCountdownChanged
        );

        CachedGameState->OnPlayerCountChanged.AddDynamic(
            this, &ULobbyViewModel::HandlePlayerCountChanged
        );
    }
}

void ULobbyViewModel::CleanupViewModel()
{
    // 부모 클래스의 정리 호출
    Super::CleanupViewModel();

    // 1. [TODO]: InitializeViewModel에서 구독했던 모든 델리게이트를 여기서 해제해야 합니다.

    // 2. 참조 해제
    if (CachedGameState)
    {
        CachedGameState->OnCountdownChanged.RemoveAll(this);
    }
    
    CachedGameState = nullptr;
}



void ULobbyViewModel::SetReadyCountText(const FText& NewText)
{
    if (!ReadyCountText.EqualTo(NewText))
    {
        ReadyCountText = NewText;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ReadyCountText);
    }
}


void ULobbyViewModel::SetTimerText(const FText& NewText)
{
    if (!TimerText.EqualTo(NewText))
    {
        TimerText = NewText;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(TimerText);
    }
}


void ULobbyViewModel::HandleCountdownChanged(int32 NewSeconds)
{
    // GameState에서 시간이 변경될 때마다 이 함수가 호출됩니다.

    // 타이머 텍스트 업데이트 (UMG 자동 갱신)
    SetTimerText(FText::Format(NSLOCTEXT("Lobby", "TimerFormat", "남은 시간: {0}"), FText::AsNumber(NewSeconds)));
}

void ULobbyViewModel::HandlePlayerCountChanged()
{
    if (CachedGameState)
    {
        int32 Ready = CachedGameState->ReadyPlayers;
        int32 Connected = CachedGameState->ConnectedPlayers;

        // FText 형식: "Ready / Connected" (예: 3/5)
        FText FormattedText = FText::Format(
            NSLOCTEXT("Lobby", "ReadyCountFormat", "{0} / {1}"),
            FText::AsNumber(Ready),
            FText::AsNumber(Connected)
        );

        SetReadyCountText(FormattedText);
    }
}


#pragma region CharSellectRegion
void ULobbyViewModel::SelectCharacter(int32 CharIndex)
{
    // ViewModel이 캐시된 Controller를 통해 Server RPC를 호출합니다.
    if (CachedPlayerController)
    {
        // ATTTPlayerController에 ServerSelectCharacter가 있다고 가정합니다.
        CachedPlayerController->ServerSelectCharacterNew(CharIndex);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::SelectCharacter: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::ConfirmSelection()
{
    if (!CachedPlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("LobbyViewModel: CachedPlayerController is NULL. Cannot process ConfirmSelection."));
        return;
    }

    ATTTPlayerState* TTTPlayerState = CachedPlayerController->GetPlayerState<ATTTPlayerState>();

    if (TTTPlayerState)
    {
        TTTPlayerState->ToggleReady();

        // 로그를 통해 레디 요청이 전달되었는지 확인 (선택 사항)
        UE_LOG(LogTemp, Log, TEXT("LobbyViewModel: Toggle Ready request sent for Player: %s"),
            *TTTPlayerState->GetPlayerName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LobbyViewModel: Failed to cast PlayerState to ATTTPlayerState."));
    }
}
#pragma endregion
