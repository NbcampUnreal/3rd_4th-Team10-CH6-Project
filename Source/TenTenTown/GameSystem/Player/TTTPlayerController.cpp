// TTTPlayerController.cpp


#include "GameSystem/Player/TTTPlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "UObject/ConstructorHelpers.h"

ATTTPlayerController::ATTTPlayerController()
{
}

void ATTTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && HUDClass && !HUDInstance)
	{
		HUDInstance = CreateWidget<UUserWidget>(this, HUDClass);
		if (HUDInstance)
		{
			HUDInstance->AddToViewport();
		}
	}
}

void ATTTPlayerController::HostLobbyCmd(int32 Port)
{
	if (UWorld* World = GetWorld())
	{
		if (auto* GI = World->GetGameInstance<UTTTGameInstance>())
		{
			GI->HostLobby(Port);
		}
	}
}

void ATTTPlayerController::JoinLobbyCmd(const FString& IP, int32 Port)
{
	if (UWorld* World = GetWorld())
	{
		if (auto* GI = World->GetGameInstance<UTTTGameInstance>())
		{
			GI->JoinLobby(IP, Port);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[JoinLobbyCmd] GameInstance is NOT UTTTGameInstance in this PIE world"));
		}
	}
}

void ATTTPlayerController::ShowEffectivePort(int32 Port)
{
	if (UWorld* World = GetWorld())
	{
		if (auto* GI = World->GetGameInstance<UTTTGameInstance>())
		{
			const int32 Effective = GI->GetEffectivePort(Port);
			UE_LOG(LogTemp, Log, TEXT("[Port] Effective=%d (Input=%d)"), Effective, Port);
		}
	}
}



