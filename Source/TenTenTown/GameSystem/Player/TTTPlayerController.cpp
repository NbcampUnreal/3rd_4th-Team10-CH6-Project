// TTTPlayerController.cpp


#include "GameSystem/Player/TTTPlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/PS/TTTPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

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
	OpenCharacterSelectUI();
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
	if (!HasAuthority() || CharClass == nullptr)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	// 기존 Pawn 제거
	if (APawn* ExistingPawn = GetPawn())
	{
		ExistingPawn->Destroy();
	}

	// PlayerStart 찾기
	APlayerStart* PlayerStart = Cast<APlayerStart>(
		UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass())
	);

	FTransform SpawnTransform;

	if (PlayerStart)
	{
		SpawnTransform = PlayerStart->GetActorTransform();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ServerSelectCharacter] No PlayerStart found -> Using world origin"));
		SpawnTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	}

	// Pawn 스폰
	APawn* NewPawn = World->SpawnActor<APawn>(CharClass, SpawnTransform);
	if (!NewPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[ServerSelectCharacter] Pawn spawn failed! Class=%s"),
			   *GetNameSafe(*CharClass));
		return;
	}

	Possess(NewPawn);

	if (ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>())
	{
		PS->SelectedCharacterClass = CharClass;
	}
}



