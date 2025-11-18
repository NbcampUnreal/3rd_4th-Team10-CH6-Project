// TTTGameInstance.cpp


#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"

DEFINE_LOG_CATEGORY_STATIC(LogTTTGameInstance, Log, All);

int32 UTTTGameInstance::ResolvePort(int32 OverridePort) const
{
	if (OverridePort>0)
	{
		return OverridePort;
	}
	int32 CliPort = 0;
	if (FParse::Value(FCommandLine::Get(),TEXT("port="), CliPort)&&CliPort>0)
	{
		return CliPort;
	}
	return (DevListenPort > 0) ? DevListenPort : 7777;
}
bool UTTTGameInstance::ResolveLobbyMapPath(FString& OutMapPath) const
{
	OutMapPath.Empty();

	if (!LobbyMap.IsNull())
	{
		OutMapPath = LobbyMap.ToSoftObjectPath().GetAssetPathString();
	}
	if (OutMapPath.IsEmpty())
	{
		OutMapPath = FallbackLobbyMapPath;
	}
	if (OutMapPath.IsEmpty())
	{
		UE_LOG(LogTTTGameInstance, Error, TEXT("[ResolveLobbyMapPath]:맵 경로 없음 비상..!"))
		return false;
	}
	return true;
}

void UTTTGameInstance::SaveSelectedCharacter(const FString& PlayerName, TSubclassOf<APawn> CharacterClass)
{
	if (PlayerName.IsEmpty() || !*CharacterClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[GameInstance::SaveSelectedCharacter] Invalid data. Name=%s Class=%s"),
			*PlayerName, *GetNameSafe(*CharacterClass));
		return;
	}

	SelectedCharacters.Add(PlayerName, CharacterClass);

	UE_LOG(LogTemp, Warning,
		TEXT("[GameInstance::SaveSelectedCharacter] Saved %s -> %s"),
		*PlayerName, *GetNameSafe(*CharacterClass));
}

TSubclassOf<APawn> UTTTGameInstance::GetSelectedCharacter(const FString& PlayerName) const
{
	if (const TSubclassOf<APawn>* Found = SelectedCharacters.Find(PlayerName))
	{
		return *Found;
	}

	return nullptr;
}

void UTTTGameInstance::HostLobby(int32 OverridePort)
{
	UWorld* World = GetWorld();
	if (!World) { UE_LOG(LogTTTGameInstance, Warning, TEXT("[HostLobby] World is null")); return; }

	FString MapPath;
	if (!ResolveLobbyMapPath(MapPath))
	{
		UE_LOG(LogTTTGameInstance, Error, TEXT("[HostLobby] Lobby map path resolve failed"));
		return;
	}

	const int32 Port = ResolvePort(OverridePort);
	const FString URL = FString::Printf(TEXT("%s?listen -port=%d"), *MapPath, Port);

	const ENetMode NM = World->GetNetMode();
	if (NM == NM_DedicatedServer || NM == NM_ListenServer)
	{
		UE_LOG(LogTTTGameInstance, Log, TEXT("[HostLobby] ServerTravel -> %s"), *URL);
		World->ServerTravel(URL, false);
	}
	else
	{
		UE_LOG(LogTTTGameInstance, Log, TEXT("[HostLobby] OpenLevel -> %s"), *URL);
		UGameplayStatics::OpenLevel(World, FName(*URL));
	}
}


void UTTTGameInstance::JoinLobby(const FString& IP, int32 OverridePort)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[JoinLobby]:월드가 없슴다"));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		UE_LOG(LogTTTGameInstance, Warning, TEXT("[JoinLobby]:플레이어컨트롤러 어디갔어;"));
		return;
	}
	const int32 Port = ResolvePort(OverridePort);
	const FString TargetIP = IP.IsEmpty() ? TEXT("127.0.0.1") : IP;
	const FString URL = FString::Printf(TEXT("%s:%d"), *TargetIP, Port);

	UE_LOG(LogTTTGameInstance, Log, TEXT("[JoinLobby]:ClientTravel URL:%s"), *URL);
	PC->ClientTravel(URL, TRAVEL_Absolute);
	
}





