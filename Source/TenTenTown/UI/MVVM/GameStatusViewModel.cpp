#include "UI/MVVM/GameStatusViewModel.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"

UGameStatusViewModel::UGameStatusViewModel()
{
    // 초기화 시점에는 별도 로직 없음
}

// ----------------------------------------------------------------------
// 초기화 및 정리
// ----------------------------------------------------------------------

void UGameStatusViewModel::InitializeViewModel(ATTTGameStateBase* GameState, UAbilitySystemComponent* InASC) // ⭐ 2개 인수로 수정
{
    CachedGameState = GameState;
    // ⭐ 인수가 2개이므로 GameState와 InASC를 모두 검사
    if (!CachedGameState || !InASC) return;

    // --- (이후의 모든 로직은 기존 코드를 그대로 유지) ---
    // 1. 초기 값 설정
    SetCoreHealth(CachedGameState->GetCoreHealth());
    SetWaveLevel(CachedGameState->GetWaveLevel());
    SetRemainEnemy(CachedGameState->GetRemainEnemy());
    SetRemainingTimeText(FormatTime(CachedGameState->GetRemainingTime()));

    // 2. GameState 델리게이트 구독
    CachedGameState->OnCoreHealthChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnCoreHealthChanged);
    CachedGameState->OnRemainingTimeChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnWaveTimerChanged);
    CachedGameState->OnWaveLevelChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnWaveLevelChanged);
    CachedGameState->OnRemainEnemyChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnRemainEnemyChanged);
}

void UGameStatusViewModel::CleanupViewModel()
{
    if (CachedGameState)
    {
        // 구독 해제
        CachedGameState->OnCoreHealthChangedDelegate.RemoveAll(this);
        CachedGameState->OnRemainingTimeChangedDelegate.RemoveAll(this);
        CachedGameState->OnWaveLevelChangedDelegate.RemoveAll(this);
        CachedGameState->OnRemainEnemyChangedDelegate.RemoveAll(this);
    }
    CachedGameState = nullptr;
    // Super::CleanupViewModel(); // UBaseViewModel에 있다면 호출
}

// ----------------------------------------------------------------------
// 델리게이트 콜백 함수 (데이터 수신)
// ----------------------------------------------------------------------

void UGameStatusViewModel::OnCoreHealthChanged(int32 NewCoreHealth)
{
    // 받은 데이터를 UPROPERTY Setter를 통해 UI에 브로드캐스트
    SetCoreHealth(NewCoreHealth);
}

void UGameStatusViewModel::OnWaveTimerChanged(int32 NewRemainingTime)
{
    // 시간을 FText로 변환하여 UI에 브로드캐스트 (ViewModel의 데이터 포맷팅 책임)
    SetRemainingTimeText(FormatTime(NewRemainingTime));
}

void UGameStatusViewModel::OnWaveLevelChanged(int32 NewWaveLevel)
{
    SetWaveLevel(NewWaveLevel);
}

void UGameStatusViewModel::OnRemainEnemyChanged(int32 NewRemainEnemy)
{
    SetRemainEnemy(NewRemainEnemy);
}

// ----------------------------------------------------------------------
// 데이터 포맷팅 로직 (ViewModel의 책임)
// ----------------------------------------------------------------------

FText UGameStatusViewModel::FormatTime(int32 TimeInSeconds) const
{
    int32 Minutes = TimeInSeconds / 60;
    int32 Seconds = TimeInSeconds % 60;

    // FText::Format을 사용하여 "MM:SS" 형태로 포맷팅합니다.
    FString FormattedString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
    return FText::FromString(FormattedString);
}


// ----------------------------------------------------------------------
// UPROPERTY Setter 구현 (FieldNotify 브로드캐스트)
// ----------------------------------------------------------------------

void UGameStatusViewModel::SetCoreHealth(int32 NewValue)
{
    if (CoreHealth != NewValue)
    {
        CoreHealth = NewValue;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CoreHealth);
    }
}

void UGameStatusViewModel::SetWaveLevel(int32 NewValue)
{
    if (WaveLevel != NewValue)
    {
        WaveLevel = NewValue;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(WaveLevel);
    }
}

void UGameStatusViewModel::SetRemainingTimeText(FText NewText)
{
    // FText 비교는 복잡하므로, 단순 대입 후 브로드캐스트하는 방식을 사용합니다.
    RemainingTimeText = NewText;
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemainingTimeText);
}

void UGameStatusViewModel::SetRemainEnemy(int32 NewValue)
{
    if (RemainEnemy != NewValue)
    {
        RemainEnemy = NewValue;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(RemainEnemy);
    }
}