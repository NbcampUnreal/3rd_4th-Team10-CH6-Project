#include "UI/MVVM/GameStatusViewModel.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UI/Map/MiniMapCamera.h"
#include "Components/PanelWidget.h"
#include "UI/MVVM/MapIconViewModel.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/Texture2D.h"

UGameStatusViewModel::UGameStatusViewModel()
{
    // 초기화 시점에는 별도 로직 없음
}

void UGameStatusViewModel::InitializeViewModel()
{
}

void UGameStatusViewModel::InitializeViewModel(ATTTGameStateBase* GameState, UAbilitySystemComponent* InASC) // ⭐ 2개 인수로 수정
{	
    CachedGameState = GameState;
    // ⭐ 인수가 2개이므로 GameState와 InASC를 모두 검사
    if (!CachedGameState || !InASC) return;

	
    // 1. 초기 값 설정
    SetCoreHealth(CachedGameState->GetCoreHealth());
    SetWaveLevel(CachedGameState->GetWaveLevel());
    SetRemainEnemy(CachedGameState->GetRemainEnemy());
    SetRemainingTimeText(FormatTime(CachedGameState->GetRemainingTime()));

    // 2. GameState 델리게이트 구독
    //CachedGameState->OnCoreHealthChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnCoreHealthChanged);
    //CachedGameState->OnRemainingTimeChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnWaveTimerChanged);
    CachedGameState->OnRemainingTimeChanged.AddDynamic(this, &UGameStatusViewModel::OnWaveTimerChanged);
    CachedGameState->OnWaveLevelChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnWaveLevelChanged);
    CachedGameState->OnRemainEnemyChangedDelegate.AddUObject(this, &UGameStatusViewModel::OnRemainEnemyChanged);
    CachedGameState->OnCoreHealthUpdated.AddDynamic(this, &UGameStatusViewModel::UpdateCoreHealthUI);

    //CreateMapIconVMs(4);
}

void UGameStatusViewModel::CleanupViewModel()
{
    if (CachedGameState)
    {
        // 구독 해제
        CachedGameState->OnCoreHealthUpdated.RemoveAll(this);
        CachedGameState->OnRemainingTimeChanged.RemoveAll(this);
        CachedGameState->OnWaveLevelChangedDelegate.RemoveAll(this);
        CachedGameState->OnRemainEnemyChangedDelegate.RemoveAll(this);
    }
    CachedGameState = nullptr;
    // Super::CleanupViewModel(); // UBaseViewModel에 있다면 호출
}


void UGameStatusViewModel::OnWaveTimerChanged(int32 NewRemainingTime)
{
    SetRemainingTimeText(FormatTime(CachedGameState->RemainingTime));
    //SetRemainingTimeText(FormatTime(NewRemainingTime));
}

void UGameStatusViewModel::OnWaveLevelChanged(int32 NewWaveLevel)
{
    SetWaveLevel(NewWaveLevel);
}

void UGameStatusViewModel::OnRemainEnemyChanged(int32 NewRemainEnemy)
{
    SetRemainEnemy(NewRemainEnemy);
}
void UGameStatusViewModel::UpdateCoreHealthUI(float NewHealth, float NewMaxHealth)
{
	SetCoreHealth(static_cast<int32>(NewHealth));
}


FText UGameStatusViewModel::FormatTime(int32 TimeInSeconds) const
{
    int32 Minutes = TimeInSeconds / 60;
    int32 Seconds = TimeInSeconds % 60;

    // FText::Format을 사용하여 "MM:SS" 형태로 포맷팅합니다.
    FString FormattedString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
    return FText::FromString(FormattedString);
}





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



    //-------------미니맵

UMapIconViewModel* UGameStatusViewModel::GetAvailableVM()
{
    for (UMapIconViewModel* VM : MapIconVMs)
    {
        if (VM && !VM->bIsBusy)
        {
            VM->bIsBusy = true; // 사용 중으로 표시
            return VM;
        }
    }
    return nullptr; // 모든 VM이 사용 중이면 null 반환
}

void UGameStatusViewModel::SetMinimapCamera(AMiniMapCamera* InCamera)
{
    CachedMinimapCamera = InCamera;
}

void UGameStatusViewModel::CreateMapIconVMs(int32 CreateCount)
{
    MapIconVMs.Empty();
    PlayerIconMap.Empty();

    for (int32 i = 0; i < CreateCount; ++i)
    {
        UMapIconViewModel* NewIconVM = NewObject<UMapIconViewModel>(this);
        if (NewIconVM)
        {
            MapIconVMs.Add(NewIconVM);
        }
	}

}

void UGameStatusViewModel::StartMinimapUpdate()
{
    if (!IsValid(CachedMinimapCamera))
    {
        UE_LOG(LogTemp, Error, TEXT("Minimap Error: CachedMinimapCamera is NULL!"));
        return;
    }

    // 2. 만약 루프 안에서 CachedGameState를 쓴다면 이것도 체크
    if (!IsValid(CachedGameState))
    {
        UE_LOG(LogTemp, Error, TEXT("Minimap Error: CachedGameState is NULL!"));
        return;
    }
	GetWorld()->GetTimerManager().SetTimer(MinimapUpdateTimer, this, &UGameStatusViewModel::SetMapIconVMs, 0.1f, true);
}

void UGameStatusViewModel::SetMapIconVMs()
{
    if (!CachedGameState || !CachedMinimapCamera) return;

    //현재 존재하는 플레이어 명단 가져오기
    TArray<APlayerState*> CurrentPSs = CachedGameState->PlayerArray;	

    //명단에 있는 플레이어들 업데이트 및 할당
    for (APlayerState* PS : CurrentPSs)
    {
        if (!PS) continue;

        UMapIconViewModel* TargetVM = nullptr;

        // [기존 매핑 확인]
        if (PlayerIconMap.Contains(PS))
        {
            TargetVM = PlayerIconMap[PS];
        }
        // [신규 할당] 매핑에 없다면 풀(MapIconVMs)에서 비어있는 VM 찾기
        else
        {
            for (UMapIconViewModel* VM : MapIconVMs)
            {
                // 어떤 PS도 이 VM을 사용하고 있지 않은지 체크
                bool bIsAlreadyUsed = false;
                for (auto& Pair : PlayerIconMap)
                {
                    if (Pair.Value == VM) { bIsAlreadyUsed = true; break; }
                }

                if (!bIsAlreadyUsed)
                {
                    TargetVM = VM;
                    PlayerIconMap.Add(PS, TargetVM);
                    break;
                }
            }
        }

        // [공통 업데이트]
        if (TargetVM)
        {
            InitializeIconVM(TargetVM, PS);
        }
    }

    //PlayerIconMap 정리 (현재 명단(CurrentPSs)에 없는 데이터 제거)
    TArray<APlayerState*> KeysToRemove;
    for (auto& Elem : PlayerIconMap)
    {
        // 맵에는 있는데 현재 서버 명단(CurrentPSs)에는 없다면? -> 나간 플레이어
        if (!CurrentPSs.Contains(Elem.Key))
        {
            if (Elem.Value)
            {
                Elem.Value->SetbIsVisible(ESlateVisibility::Collapsed);
            }
            KeysToRemove.Add(Elem.Key);
        }
    }

    // 실제 제거
    for (APlayerState* PSKey : KeysToRemove)
    {
        PlayerIconMap.Remove(PSKey);
    }
}

void UGameStatusViewModel::InitializeIconVM(UMapIconViewModel* VM, APlayerState* PS)
{
    APawn* P = PS->GetPawn();
    if (P)
    {
        VM->SetbIsVisible(ESlateVisibility::Visible);

        // 위치 업데이트
        FVector2D NewPos = CachedMinimapCamera->GetPlayerMiniMapPosition(P->GetActorLocation());
        VM->SetIconPosition(NewPos);

        // 텍스처가 아직 없다면 (멀티플레이어 복제 지연 대응)
        if (ABaseCharacter* BC = Cast<ABaseCharacter>(P))
        {
            VM->SetIconTexture(BC->CharacterIconTexture.Get());
        }

        //ps가 나 자신일경우
		APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
        if (LocalPC && LocalPC->PlayerState == PS)
        {
            VM->SetbIsMyPlayerIcon(ESlateVisibility::Visible);
        }
        else
        {
            VM->SetbIsMyPlayerIcon(ESlateVisibility::Collapsed);
		}
    }
    else
    {
        VM->SetbIsVisible(ESlateVisibility::Collapsed);
    }
}
