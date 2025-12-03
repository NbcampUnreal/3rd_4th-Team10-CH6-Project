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

	/*/** 위젯에서 부르는 서버 RPC (BP 노드에서 보이게 BlueprintCallable은 안 붙여도 됨) #1#
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSelectCharacter(TSubclassOf<APawn> CharClass);*/
	
#pragma region UI_Region
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSelectCharacterNew(int32 CharIndex);

	virtual void OnRep_PlayerState() override;

	UFUNCTION(Server, Reliable)
	void ServerOpenCharacterSelectUI();

	UFUNCTION(Server, Reliable)
	void ServerOpenMapSelectUI();
#pragma endregion




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

	// ==== 로비 UI 상태 요청 & 응답용 RPC ====
	UFUNCTION(Server, Reliable)
	void ServerRequestLobbyUIState();   // 로비에 들어온 클라가 서버에게 "뭐 띄워?" 물어봄

	UFUNCTION(Client, Reliable)
	void ClientShowLobbyResult(const FTTTLastGameResult& Result); // 서버→클라: 결과창 띄워라

	UFUNCTION(Client, Reliable)
	void ClientShowLobbyCharacterSelect();       

	UFUNCTION(Server, Reliable)
	void Server_SelectMapIndex(int32 MapIndex);

	UFUNCTION(Exec)
	void SetMap(int32 MapIndex);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TTT|UI")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget* HUDInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TTT|UI")
	TSubclassOf<class UUserWidget> ResultWidgetClass;

	UPROPERTY()
	UUserWidget* ResultWidgetInstance = nullptr;

	// R키 입력 바인딩용
	virtual void SetupInputComponent() override;

	// R키 눌렀을 때 실행
	UFUNCTION()
	void OnReadyKeyPressed();

	void TestSelectMap0(); 
	void TestSelectMap1(); 
	void TestSelectMap2(); 
	
#pragma region LootSubSystemRPC
	// 골드 직접 설정 (Set)
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Loot|RPC")
	void ServerSetGold(int32 NewGold);

	// 골드 추가 (Add)
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Loot|RPC")
	void ServerAddGold(int32 Amount);


	// --- [EXP] 서버 요청 함수 (RPC) ---

	// 경험치 직접 설정 (Set)
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Loot|RPC")
	void ServerSetEXP(float NewEXP);

	// 경험치 추가 (Add)
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Loot|RPC")
	void ServerAddEXP(float Amount);
#pragma endregion
};
