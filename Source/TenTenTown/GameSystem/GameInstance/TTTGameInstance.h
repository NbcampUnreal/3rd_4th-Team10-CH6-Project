// TTTGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TTTGameInstance.generated.h"

UCLASS(Config = Game)
class TENTENTOWN_API UTTTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Net")
	int32 DevListenPort = 7777;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Net")
	TSoftObjectPtr<UWorld> LobbyMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Net")
	FString FallbackLobbyMapPath = TEXT("/Game/Maps/LobbyMap");

	UFUNCTION(BlueprintCallable, Category="Net")
	void HostLobby(int32 OverridePort);

	void HostLobby() { HostLobby(-1); } // C++ 편의 오버로드

	UFUNCTION(BlueprintCallable, Category="Net")
	void JoinLobby(const FString& IP, int32 OverridePort);

	void JoinLobby(const FString& IP) { JoinLobby(IP, -1); }

	UFUNCTION(BlueprintPure, Category="Net")
	int32 GetEffectivePort(int32 OverridePort) const;

	UFUNCTION(Exec)
	void JoinLobbyExec(const FString& IP, int32 Port = -1);

private:
	int32 ResolvePort(int32 OverridePort) const;
	bool ResolveLobbyMapPath(FString& OutMapPath) const;
};
