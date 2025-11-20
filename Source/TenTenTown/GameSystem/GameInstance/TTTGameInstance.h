// TTTGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TTTGameInstance.generated.h"

UCLASS(Config = Game)
class TENTENTOWN_API UTTTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// OnlineSubsystem 세팅용
	virtual void Init() override;

	// ---- 기존 Net 설정 ----
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Net")
	int32 DevListenPort = 7777;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Net")
	TSoftObjectPtr<UWorld> LobbyMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Net")
	FString FallbackLobbyMapPath = TEXT("/Game/Maps/LobbyMap");

	// IP/포트 기반 호스트 (세션 내부에서 재사용)
	UFUNCTION(BlueprintCallable, Category="Net")
	void HostLobby(int32 OverridePort);
	void HostLobby() { HostLobby(-1); }

	// IP/포트 기반 조인 (세션 내부/디버그 용도로 재사용 가능)
	UFUNCTION(BlueprintCallable, Category="Net")
	void JoinLobby(const FString& IP, int32 OverridePort);
	void JoinLobby(const FString& IP) { JoinLobby(IP, -1); }

	UFUNCTION(BlueprintPure, Category="Net")
	int32 GetEffectivePort(int32 OverridePort) const;

	// ---- OnlineSession API (스타트 UI에서 호출) ----
	/** Host 버튼에서 호출: 세션 만들고 로비맵으로 서버 트래블 */
	UFUNCTION(BlueprintCallable, Category="OnlineSession")
	void CreateOrHostSession(bool bDedicated = false);

	/** Join 버튼에서 호출: 세션 검색 후 첫 번째 세션에 조인 */
	UFUNCTION(BlueprintCallable, Category="OnlineSession")
	void FindSessionsAndJoin();

	// Start 메뉴에서 Apply 눌렀을 때: IP/Port를 저장만 하는 함수
	UFUNCTION(BlueprintCallable, Category="Net")
	void SaveConnectInfo(const FString& InIP, int32 InPort);

	// Start 메뉴에서 Play 눌렀을 때: 저장된 IP/Port로 접속하는 함수
	UFUNCTION(BlueprintCallable, Category="Net")
	void JoinSavedLobby();

	// ---- 캐릭터 선택 정보 저장 ----
	// 플레이어 이름 기준으로 선택한 캐릭터 기록
	void SaveSelectedCharacter(const FString& PlayerName, TSubclassOf<APawn> CharacterClass);
	TSubclassOf<APawn> GetSelectedCharacter(const FString& PlayerName) const;

protected:
	// Dedicated 서버에서 자동 호출용
	void HostDedicatedSession();

	// OnlineSession delegates
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	// OnlineSession 핸들들
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	int32 ResolvePort(int32 OverridePort) const;
	bool ResolveLobbyMapPath(FString& OutMapPath) const;

	// 서버에서만 의미 있는 데이터 (SeamlessTravel 동안 유지)
	UPROPERTY()
	TMap<FString, TSubclassOf<APawn>> SelectedCharacters;

	// Apply 버튼으로 저장해두는 IP/Port
	UPROPERTY()
	FString SavedConnectIP;

	UPROPERTY()
	int32 SavedConnectPort = -1;


#pragma region UI_Region
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<APawn>> AvailableCharacterClasses;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* StructureDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* ItemDataTable;
#pragma endregion



};
