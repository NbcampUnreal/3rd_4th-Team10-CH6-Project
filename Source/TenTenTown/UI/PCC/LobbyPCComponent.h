#pragma once

#include "CoreMinimal.h"
#include "UI/PCC/PCCBase.h"
#include "LobbyPCComponent.generated.h"

class UCharSellectWidget;
class ULobbyWidget;
class ULobbyViewModel;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TENTENTOWN_API ULobbyPCComponent : public UPCCBase
{
	GENERATED_BODY()

public:
	ULobbyPCComponent();

protected:	
	virtual void BeginPlay() override;
public:
	void ReBeginPlay();

protected:
	FTimerHandle InitCheckTimerHandle;
	UPROPERTY()
	TObjectPtr<ULobbyViewModel> LobbyRootViewModel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Widgets")
	TSubclassOf<UCharSellectWidget> CharSellectWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Widgets")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<UCharSellectWidget> CharSellectWidgetInstance;
	UPROPERTY()
	TObjectPtr<ULobbyWidget> LobbyWidgetInstance;


	// *** [GAS] OnModeTagChanged 함수 오버라이드 ***
	// 부모 클래스의 가상 함수를 재정의하여 태그 변화에 반응합니다.
	virtual void OnModeTagChanged(const FGameplayTag Tag, int32 NewCount) override;

private:
	// OnModeTagChanged를 대신하여 Lobby 태그만 처리하는 전용 콜백
	void OnLobbyTagChanged(const FGameplayTag Tag, int32 NewCount);

public:
	// UI 생성/파괴 로직을 분리
	void OpenLobbyUI();
	void CloseLobbyUI();


protected:
	
	//캐릭터 선택 태그 변화를 처리할 새로운 함수 선언
	UFUNCTION()
	void OnCharacterSelectionTagChanged(const FGameplayTag Tag, int32 NewCount);
};