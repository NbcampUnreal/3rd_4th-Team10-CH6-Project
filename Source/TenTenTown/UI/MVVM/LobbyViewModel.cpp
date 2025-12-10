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
        CachedGameState->OnSelectedMapChanged.AddDynamic(
            this, &ULobbyViewModel::HandleSelectedMapChanged
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
void ULobbyViewModel::SetMapIconTexture(UTexture2D* NewTexture)
{
    UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: SetMapIconTexture called with NewTexture"));
	
    if (MapIconTexture != NewTexture)
    {
		UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: Updating MapIconTexture"));
        MapIconTexture = NewTexture;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MapIconTexture);
    }
}


void ULobbyViewModel::HandleCountdownChanged(int32 NewSeconds)
{
    // GameState에서 시간이 변경될 때마다 이 함수가 호출됩니다.

    // 타이머 텍스트 업데이트 (UMG 자동 갱신)
    SetTimerText(FText::Format(NSLOCTEXT("Lobby", "TimerFormat", "STARTING IN: {0}"), FText::AsNumber(NewSeconds)));
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

void ULobbyViewModel::HandleSelectedMapChanged(int32 NewMapIndex)
{
	UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: HandleSelectedMapChanged called with NewMapIndex=%d"), NewMapIndex);
    //게임 인스턴스에서 인덱스 번호로 텍스쳐를 가져와라
    if (!CachedPlayerController) { return; }

	UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: CachedPlayerController is valid."));
    UTTTGameInstance* TTTGI = CachedPlayerController->GetGameInstance<UTTTGameInstance>();
    if (!TTTGI) { return; }
	UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: TTTGI is valid."));
	UTexture2D* NewIcon = TTTGI->GetMapIconByIndex(NewMapIndex);
    
    SetMapIconTexture(NewIcon);
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

#pragma region MapSelectRegion
void ULobbyViewModel::SelectMap(int32 CharIndex)
{
    // ViewModel이 캐시된 Controller를 통해 Server RPC를 호출합니다.
    if (CachedPlayerController)
    {
        // ATTTPlayerController에 ServerSelectCharacter가 있다고 가정합니다.
        CachedPlayerController->SetMap(CharIndex);
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::SelectMap: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::ReSelectCharacter()
{
	UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: ReSelectCharacter called."));
    if (CachedPlayerController)
    {
        // 서버 RPC를 호출하여 ASC에 태그를 부여하고, 
        // 결과적으로 ULobbyPCComponent의 OnCharacterSelectionTagChanged가 호출되어 UI가 열림
        CachedPlayerController->ServerOpenCharacterSelectUI();

        UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: Requested Character Re-Selection UI from Server."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::ReSelectCharacter: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::ReSelectMap()
{
	UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: ReSelectMap called."));
    if (CachedPlayerController)
    {
        // 서버 RPC를 호출하여 ASC에 태그를 부여하고, 
        // 결과적으로 ULobbyPCComponent의 OnCharacterSelectionTagChanged가 호출되어 UI가 열림
        CachedPlayerController->ServerOpenMapSelectUI();

		UE_LOG(LogTemp, Log, TEXT("ULobbyViewModel: Requested Map Re-Selection UI from Server."));
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("ULobbyViewModel::ReSelectMap: PlayerController is not cached!"));
    }
}
void ULobbyViewModel::SetMapButtonVisibility(const ESlateVisibility NewVisibility)
{
    if (!bIsHost && NewVisibility == ESlateVisibility::Visible)
    {
		MapButtonVisibility = ESlateVisibility::Hidden;        
    }
    else
    {
        MapButtonVisibility = NewVisibility;
    }
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MapButtonVisibility);
    
}

#pragma endregion

void ULobbyViewModel::SetIsHost(bool bNewIsHost)
{
    if (bIsHost != bNewIsHost)
    {
        bIsHost = bNewIsHost;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsHost);
	}
}
