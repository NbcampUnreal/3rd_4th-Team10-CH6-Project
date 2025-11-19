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

	// 2) GameInstance에 선택한 캐릭터 저장 (인게임용 진짜 데이터)
	if (UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>())
	{
		GI->SaveSelectedCharacter(PlayerName, CharClass);
	}

	// 3) PlayerState에도 기록 (UI/디버그용)
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

	// 4) **로비맵일 때만 프리뷰용 Pawn 스폰**
	const FString MapName = World->GetMapName(); // 예: UEDPIE_0_LobbyMap
	if (MapName.Contains(TEXT("LobbyMap")))
	{
		// 기존 Pawn 있으면 제거 (다른 캐릭 골랐을 때 교체)
		if (APawn* ExistingPawn = GetPawn())
		{
			ExistingPawn->Destroy();
		}

		// PlayerStart 찾기
		AActor* StartSpotActor = UGameplayStatics::GetActorOfClass(
			World,
			APlayerStart::StaticClass()
		);

		FTransform SpawnTransform = StartSpotActor
			? StartSpotActor->GetActorTransform()
			: FTransform(FRotator::ZeroRotator, FVector::ZeroVector);

		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = nullptr;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APawn* NewPawn = World->SpawnActor<APawn>(
			CharClass,
			SpawnTransform,
			Params
		);

		if (!NewPawn)
		{
			UE_LOG(LogTemp, Error,
				TEXT("[ServerSelectCharacter] Lobby PREVIEW spawn failed! Class=%s"),
				*GetNameSafe(*CharClass));
			return;
		}

		Possess(NewPawn);

		UE_LOG(LogTemp, Warning,
			TEXT("[ServerSelectCharacter] Lobby PREVIEW Spawned Pawn=%s for PC=%s (Map=%s)"),
			*GetNameSafe(NewPawn),
			*GetNameSafe(this),
			*MapName);
	}
	// InGameMap에서는 여기 코드가 실행되지 않음 → 인게임 스폰은 GameMode가 담당
}






