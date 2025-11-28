#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "GameStatusViewModel.generated.h"


class ATTTGameStateBase;
class UAbilitySystemComponent;

UCLASS()
class TENTENTOWN_API UGameStatusViewModel : public UBaseViewModel
{
    GENERATED_BODY()

public:
    UGameStatusViewModel();

    // UPlayPCComponent에서 호출하여 GameState에 연결하고 구독을 설정하는 함수
    void InitializeViewModel(ATTTGameStateBase* GameState, UAbilitySystemComponent* ASC);

    // PC Component 종료 시 구독을 해제하고 정리하는 함수
    void CleanupViewModel();

    //GameState 델리게이트 콜백 함수들
    // (ATTTGameStateBase에 이 이름의 델리게이트가 정의되어 있다고 가정합니다.)
    void OnCoreHealthChanged(int32 NewCoreHealth);
    void OnWaveTimerChanged(int32 NewRemainingTime);
    void OnWaveLevelChanged(int32 NewWaveLevel);
    void OnRemainEnemyChanged(int32 NewRemainEnemy);

    // --- UPROPERTY (UI 바인딩 소스) ---

    // 코어 체력
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    int32 CoreHealth = 10;

    // 웨이브 레벨
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    int32 WaveLevel = 1;

    // 남은 시간 (타이머)
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    FText RemainingTimeText; // 시간을 FText로 변환하여 UI에 전달 (분:초 포맷팅을 위해)

    // 남은 적 수
    UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
    int32 RemainEnemy = 0;

protected:
    UPROPERTY()
    TObjectPtr<ATTTGameStateBase> CachedGameState;

    // --- Getter & Setter 구현 ---

    UFUNCTION()
    int32 GetCoreHealth() const { return CoreHealth; }
    UFUNCTION()
    void SetCoreHealth(int32 NewValue); // UPROPERTY Setter

    UFUNCTION()
    int32 GetWaveLevel() const { return WaveLevel; }
    UFUNCTION()
    void SetWaveLevel(int32 NewValue); // UPROPERTY Setter

    UFUNCTION()
    FText GetRemainingTimeText() const { return RemainingTimeText; }
    UFUNCTION()
    void SetRemainingTimeText(FText NewText); // UPROPERTY Setter

    UFUNCTION()
    int32 GetRemainEnemy() const { return RemainEnemy; }
    UFUNCTION()
    void SetRemainEnemy(int32 NewValue); // UPROPERTY Setter

    // 시간을 FText로 포맷팅하는 내부 로직
    FText FormatTime(int32 TimeInSeconds) const;
};