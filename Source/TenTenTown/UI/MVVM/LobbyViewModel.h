#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "LobbyViewModel.generated.h"

// Forward Declaration
class ATTTPlayerState;
class ALobbyGameState;
class ATTTPlayerController;

UCLASS(BlueprintType)
class TENTENTOWN_API ULobbyViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	// **로비 뷰모델 초기화:** PCC에서 PlayerState를 받아와 구독을 시작합니다.
	void Initialize(ALobbyGameState* InGS, ATTTPlayerController* InPC);

	// UBaseViewModel의 핵심 함수 오버라이드
	virtual void InitializeViewModel() override;
	virtual void CleanupViewModel() override;

protected:
	// GameState 참조 포인터
	TObjectPtr<class ALobbyGameState> CachedGameState;

	UPROPERTY(BlueprintGetter = GetReadyCountText, FieldNotify)
	FText ReadyCountText;

public:
	UFUNCTION(BlueprintPure)
	FText GetReadyCountText() const { return ReadyCountText; }

protected:
	UPROPERTY(BlueprintGetter = GetTimerText, FieldNotify)
	FText TimerText;

public:
	UFUNCTION(BlueprintPure)
	FText GetTimerText() const { return TimerText; }

	// 뷰모델에서 이 값을 업데이트할 Setter 함수 (C++ 전용)
	void SetReadyCountText(const FText& NewText);
	void SetTimerText(const FText& NewText);




	// GameState 델리게이트 구독 핸들러 함수
	UFUNCTION()
	void HandleCountdownChanged(int32 NewSeconds);

	UFUNCTION()
	void HandlePlayerCountChanged();


	

#pragma region CharSellectRegion
protected:
	TObjectPtr<class ATTTPlayerController> CachedPlayerController;
public:
	UFUNCTION(BlueprintCallable, Category = "Lobby|Character")
	void SelectCharacter(int32 CharIndex);
	UFUNCTION(BlueprintCallable, Category = "Lobby|Character")
	void ConfirmSelection();
#pragma endregion



};