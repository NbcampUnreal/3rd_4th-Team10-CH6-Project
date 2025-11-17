// TTTPlayerController.cpp


#include "GameSystem/Player/TTTPlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/PS/TTTPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameSystem/GameMode/TTTGameModeBase.h"

ATTTPlayerController::ATTTPlayerController()
{
}

void ATTTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 1) HUD 생성
	if (IsLocalController() && HUDClass && !HUDInstance)
	{
		HUDInstance = CreateWidget<UUserWidget>(this, HUDClass);
		if (HUDInstance)
		{
			HUDInstance->AddToViewport();
		}
	}

	// 2) LobbyMap에서는 캐릭터 선택 UI 열기
	if (IsLocalController())
	{
		if (UWorld* World = GetWorld())
		{
			const FString MapName = World->GetMapName(); // PIE면 UEDPIE_0_LobbyMap 이런 식
			if (MapName.Contains(TEXT("LobbyMap")))
			{
				OpenCharacterSelectUI();
			}
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

void ATTTPlayerController::CloseCharacterSelectUI()
{
	if (CharacterSelectInstance && CharacterSelectInstance->IsInViewport())
	{
		CharacterSelectInstance->RemoveFromParent();
		CharacterSelectInstance = nullptr;
	}

	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
}

void ATTTPlayerController::OpenCharacterSelectUI()
{
	if (!IsLocalController() || !CharacterSelectClass)
	{
		return;
	}

	if (!CharacterSelectInstance)
	{
		CharacterSelectInstance = CreateWidget<UUserWidget>(this, CharacterSelectClass);
	}

	if (CharacterSelectInstance && !CharacterSelectInstance->IsInViewport())
	{
		CharacterSelectInstance->AddToViewport();
	}

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}

void ATTTPlayerController::ServerSelectCharacter_Implementation(TSubclassOf<APawn> CharClass)
{
	UE_LOG(LogTemp, Warning, TEXT("[ServerSelectCharacter] PC=%s  CharClass=%s"),
		*GetName(), *GetNameSafe(CharClass));

	if (!HasAuthority() || CharClass == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 1) PlayerName 가져오기
	FString PlayerName = TEXT("Unknown");
	if (APlayerState* PS_Base = PlayerState)
	{
		PlayerName = PS_Base->GetPlayerName();
	}

	// 2) GameInstance에 선택한 캐릭터 저장
	if (UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>())
	{
		GI->SaveSelectedCharacter(PlayerName, CharClass);
	}

	// 3) (선택) PlayerState에도 기록해 두면 UI/디버그에 좋음
	if (ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>())
	{
		PS->SelectedCharacterClass = CharClass;
		UE_LOG(LogTemp, Warning, TEXT("[ServerSelectCharacter] Server PS Set %s : %s"),
			*PlayerName, *GetNameSafe(PS->SelectedCharacterClass));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ServerSelectCharacter] NO PLAYERSTATE on SERVER"));
	}

	// 4) **로비 프리뷰용 스폰은 지금은 안 함** 
	//    (원한다면 여기서만 프리뷰 Pawn을 로비에 스폰하고, 인게임은 GameMode가 담당)
}




