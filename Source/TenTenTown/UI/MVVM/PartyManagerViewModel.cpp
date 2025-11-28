#include "UI/MVVM/PartyManagerViewModel.h"
#include "UI/MVVM/PartyStatusViewModel.h"
#include "GameSystem/GameMode/TTTGameStateBase.h" 
#include "Character/PS/TTTPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"



void UPartyManagerViewModel::InitializeViewModel(ATTTPlayerState* PlayerState, ATTTGameStateBase* GameState)
{
    UE_LOG(LogTemp, Log, TEXT("[PartyManagerVM] InitializeViewModel called. PlayerState: %s, GameState: %s"),
        PlayerState ? *PlayerState->GetPlayerName() : TEXT("NULL"),
		GameState ? *GameState->GetName() : TEXT("NULL"));

    // ⭐⭐ 2. PlayerState 변수를 캐싱합니다. ⭐⭐
    CachedPlayerState = PlayerState;
    CachedGameState = GameState;

    // ⭐⭐ 3. 유효성 검사: 두 인수를 모두 검사합니다. ⭐⭐
    if (!CachedPlayerState || !CachedGameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PartyManagerVM] Initialization failed: PlayerState or GameState is null."));
        return;
    }

    //// LocalPlayerState 대신 캐싱된 CachedPlayerState를 사용합니다.
    //// ATTTPlayerState* LocalPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ATTTPlayerState>(); // ⭐ 이 줄은 필요 없어짐

    //// 1. GameState 델리게이트 구독 시작
    //// ... (기존 로직 유지)
    //CachedGameState->OnPlayerJoinedDelegate.AddUObject(this, &UPartyManagerViewModel::RefreshPartyMembers);
    //CachedGameState->OnPlayerLeftDelegate.AddUObject(this, &UPartyManagerViewModel::RefreshPartyMembers);

    //// 2. 현재 접속 중인 플레이어 목록을 순회하며 초기 ViewModel 생성
    //// TArray<ATTTPlayerState*> InitialPlayers = CachedGameState->GetAllCurrentPartyMembers();

    //// 로컬 플레이어 상태를 기반으로 멤버 목록을 가져옵니다.
    //TArray<ATTTPlayerState*> InitialPlayers = CachedGameState->GetAllCurrentPartyMembers(CachedPlayerState); // ⭐ LocalPlayerState 대신 CachedPlayerState 사용

    //// InitialPlayers수 로그 출력
    //UE_LOG(LogTemp, Log, TEXT("PartyManagerViewModel Initialize: Initial Players Count = %d"), InitialPlayers.Num());
    //for (ATTTPlayerState* CurrentPlayerState : InitialPlayers) // 변수 이름 충돌 방지
    //{
    //    if (CurrentPlayerState)
    //    {
    //        AddPartyMember(CurrentPlayerState);
    //    }
    //}
    CachedGameState->OnPlayerJoinedDelegate.AddUObject(this, &UPartyManagerViewModel::HandlePlayerListUpdate);

    ResetAndRefreshAll();
}

void UPartyManagerViewModel::RefreshPartyMembers()
{
    if (!CachedGameState || !CachedPlayerState) return;

    // 1. 기존 뷰모델 정리
    for (auto& Pair : PartyViewModelMap)
    {
        if (Pair.Value) Pair.Value->CleanupViewModel();
    }
    PartyViewModelMap.Empty();
    PartyMembers.Empty();

    // 2. 현재 접속 중인 플레이어 배열 가져오기
    TArray<ATTTPlayerState*> CurrentPlayers = CachedGameState->GetAllCurrentPartyMembers(CachedPlayerState);

    // 3. 새 뷰모델 생성
    for (ATTTPlayerState* PS : CurrentPlayers)
    {
        if (!PS) continue;

        UPartyStatusViewModel* VM = NewObject<UPartyStatusViewModel>(this);
        VM->InitializeViewModel(PS);
        PartyViewModelMap.Add(PS, VM);
        PartyMembers.Add(VM);
    }

    // 4. UI 브로드캐스트
    SetPartyMembers(PartyMembers);
}

void UPartyManagerViewModel::CleanupViewModel()
{
    if (CachedGameState)
    {
        // 1. GameState 델리게이트 구독 해제
         //CachedGameState->OnPlayerJoinedDelegate.RemoveAll(this);
         //CachedGameState->OnPlayerLeftDelegate.RemoveAll(this);
    }

    // 2. 개별 PartyStatusViewModel 정리 및 목록 비우기
    for (const auto& Pair : PartyViewModelMap)
    {
        // Pair.Value (PartyStatusViewModel)의 CleanupViewModel 호출
        if (Pair.Value)
        {
            Pair.Value->CleanupViewModel();
        }
    }
    PartyViewModelMap.Empty();

    // 3. UI 목록 초기화 및 브로드캐스트
    //SetPartyMembers({});

    CachedGameState = nullptr;
}





// -----------------------------------------------------
// 목록 조작 함수 (핵심 MVVM 로직)
// -----------------------------------------------------

void UPartyManagerViewModel::AddPartyMember(ATTTPlayerState* NewPlayerState)
{
	UE_LOG(LogTemp, Log, TEXT("Adding Party Member: %s"), NewPlayerState ? *NewPlayerState->GetPlayerName() : TEXT("NULL"));
    if (!NewPlayerState || PartyViewModelMap.Contains(NewPlayerState))
    {
        return;
    }

    // 1. 새 PartyStatusViewModel 인스턴스 생성 및 초기화
    UPartyStatusViewModel* NewViewModel = NewObject<UPartyStatusViewModel>(this);
    NewViewModel->InitializeViewModel(NewPlayerState); // 여기서 GAS 구독이 시작됨

    // 2. 내부 맵과 UI 목록에 추가
    PartyViewModelMap.Add(NewPlayerState, NewViewModel);
    PartyMembers.Add(NewViewModel);

    // 3. 목록 변경 사항 UI에 브로드캐스트 (UListView 자동 갱신)
    // SetPartyMembers 호출을 통해 TArray<TObjectPtr<...>>가 변경되었음을 알립니다.
    SetPartyMembers(PartyMembers);

    UE_LOG(LogTemp, Log, TEXT("Party Member Added: %s"), *NewPlayerState->GetPlayerName());
}

void UPartyManagerViewModel::RemovePartyMember(ATTTPlayerState* LeavingPlayerState)
{
    if (!LeavingPlayerState || !PartyViewModelMap.Contains(LeavingPlayerState))
    {
        return;
    }

    // 1. 제거할 ViewModel을 맵에서 가져오기
    TObjectPtr<UPartyStatusViewModel> ViewModelToRemove = PartyViewModelMap.FindAndRemoveChecked(LeavingPlayerState);

    // 2. ViewModel 정리 (GAS 구독 해제 등)
    if (ViewModelToRemove)
    {
        ViewModelToRemove->CleanupViewModel();

        // 3. UI 목록에서 제거
        PartyMembers.Remove(ViewModelToRemove);
    }

    // 4. 목록 변경 사항 UI에 브로드캐스트 (UListView 자동 갱신)
    SetPartyMembers(PartyMembers);

    UE_LOG(LogTemp, Log, TEXT("Party Member Removed: %s"), *LeavingPlayerState->GetPlayerName());
}


// -----------------------------------------------------
// GameState 델리게이트 콜백
// -----------------------------------------------------

void UPartyManagerViewModel::HandlePlayerJoined(ATTTPlayerState* NewPlayerState)
{
    if (NewPlayerState)
    {
        UE_LOG(LogTemp, Log, TEXT("Player Joined: %s"), *NewPlayerState->GetPlayerName());
        AddPartyMember(NewPlayerState);
    }
}

void UPartyManagerViewModel::HandlePlayerLeft(ATTTPlayerState* LeavingPlayerState)
{
    if (LeavingPlayerState)
    {
        UE_LOG(LogTemp, Log, TEXT("Player Left: %s"), *LeavingPlayerState->GetPlayerName());
        RemovePartyMember(LeavingPlayerState);
    }
}


// -----------------------------------------------------
// UPROPERTY Setter 구현 (FieldNotify 브로드캐스트)
// -----------------------------------------------------


TArray<UPartyStatusViewModel*> UPartyManagerViewModel::GetPartyMembers() const
{
    // 멤버 변수 (TObjectPtr 배열)를 RAW 포인터 배열로 변환하여 반환
    TArray<UPartyStatusViewModel*> RawPtrArray;
    for (const TObjectPtr<UPartyStatusViewModel>& Member : PartyMembers)
    {
        // TObjectPtr에서 RAW 포인터를 얻습니다.
        RawPtrArray.Add(Member.Get());
    }
    return RawPtrArray;
}


void UPartyManagerViewModel::SetPartyMembers(TArray<UPartyStatusViewModel*> NewMembers)
{
    this->PartyMembers.Empty();
    for (UPartyStatusViewModel* Member : NewMembers)
    {
        this->PartyMembers.Add(Member);
    }

    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PartyMembers);
}


void UPartyManagerViewModel::ResetAndRefreshAll()
{
	UE_LOG(LogTemp, Log, TEXT("[PartyManagerVM] ResetAndRefreshAll called."));
    //// --- 1. 클린업 로직 (목록 비우기) ---
    //// (InitializeViewModel에서 구독을 설정했으므로 구독 해제는 필요 없음, 목록 데이터만 정리)

    //// 개별 PartyStatusViewModel 정리 및 맵 비우기
    //for (const auto& Pair : PartyViewModelMap)
    //{
    //    if (Pair.Value)
    //    {
    //        Pair.Value->CleanupViewModel();
    //    }
    //}
    //PartyViewModelMap.Empty();

    //// UI 목록 초기화 및 브로드캐스트 (UI에서 모든 항목 제거)
    //SetPartyMembers({});

    //// --- 2. 초기 세팅 로직 (목록 다시 채우기) ---
    //if (!CachedPlayerState || !CachedGameState)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("[PartyManagerVM] Refresh failed: Not initialized."));
    //    return;
    //}

    //// 현재 접속 중인 플레이어 목록을 다시 가져와 순회
    //TArray<ATTTPlayerState*> CurrentPlayers = CachedGameState->GetAllCurrentPartyMembers(CachedPlayerState);
    //UE_LOG(LogTemp, Log, TEXT("[PartyManagerVM] Full Refresh: Players Count = %d"), CurrentPlayers.Num());

    //for (ATTTPlayerState* CurrentPlayerState : CurrentPlayers)
    //{
    //    if (CurrentPlayerState)
    //    {
    //        // AddPartyMember 내부에서 ViewModel을 새로 생성하고 목록에 추가합니다.
    //        AddPartyMember(CurrentPlayerState);
    //    }
    //}

    if (!CachedPlayerState || !CachedGameState)
    {
        return;
    }

    // 기존 PartyStatusViewModel 정리
    for (UPartyStatusViewModel* VM : PartyMembers)
    {
        if (VM)
            VM->CleanupViewModel();
    }
    PartyMembers.Empty();

    // 전체 플레이어 목록 가져오기 (로컬 제외)
    TArray<ATTTPlayerState*> AllPlayers = CachedGameState->GetAllCurrentPartyMembers(CachedPlayerState);

    for (ATTTPlayerState* PS : AllPlayers)
    {
        if (!PS) continue;

        UPartyStatusViewModel* NewVM = NewObject<UPartyStatusViewModel>(this);
        NewVM->InitializeViewModel(PS); // GAS 바인딩 등 처리
        PartyMembers.Add(NewVM);
    }

    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PartyMembers);
	UE_LOG(LogTemp, Log, TEXT("[PartyManagerVM] Full Refresh Completed."));
}

void UPartyManagerViewModel::HandlePlayerListUpdate()
{
    ResetAndRefreshAll();
}

