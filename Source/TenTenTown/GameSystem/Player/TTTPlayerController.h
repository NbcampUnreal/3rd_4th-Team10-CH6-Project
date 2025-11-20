// TTTPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "TTTPlayerController.generated.h"


UCLASS()
class TENTENTOWN_API ATTTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATTTPlayerController();
	
	virtual void BeginPlay() override;

	/** 위젯에서 부르는 서버 RPC (BP 노드에서 보이게 BlueprintCallable은 안 붙여도 됨) */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSelectCharacter(TSubclassOf<APawn> CharClass);
	
	UPROPERTY()
	UUserWidget* CharacterSelectUI = nullptr;
	
	UFUNCTION(BlueprintCallable)
	void CloseCharacterSelectUI();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TTT|UI")
	TSubclassOf<UUserWidget> CharacterSelectClass;
	
	UPROPERTY()
	UUserWidget* CharacterSelectInstance = nullptr;

	UFUNCTION(BlueprintCallable)
	void OpenCharacterSelectUI();

	// ★ 로비에서 결과 UI를 띄우는 함수
	void ShowResultUI(const FTTTLastGameResult& Result);

	// ★ 결과창 버튼에서 호출할 함수들
	UFUNCTION(BlueprintCallable, Category="EndGame")
	void OnResultRestartClicked();

	UFUNCTION(BlueprintCallable, Category="EndGame")
	void OnResultExitClicked();

	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TTT|UI")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget* HUDInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TTT|UI")
	TSubclassOf<class UUserWidget> ResultWidgetClass;

	UPROPERTY()
	UUserWidget* ResultWidgetInstance = nullptr;
};
