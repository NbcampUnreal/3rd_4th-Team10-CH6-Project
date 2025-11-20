// TTTPlayerController.cpp


#include "GameSystem/Player/TTTPlayerController.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/PS/TTTPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetSystemLibrary.h"
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

	// 2) 로비맵에 들어왔으면, 서버에게 "무슨 UI 띄워야 하는지" 요청
	if (IsLocalController())
	{
		if (UWorld* World = GetWorld())
		{
			const FString MapName = World->GetMapName();

			// PIE에서는 UEDPIE_0_LobbyMap 이런 식으로 나오니까 Contains 사용
			if (MapName.Contains(TEXT("LobbyMap")))
			{
				// ✅ 여기서 더 이상 GameInstance 직접 안 보고, 서버한테 물어본다
				ServerRequestLobbyUIState();
			}
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
void ATTTPlayerController::ShowResultUI(const FTTTLastGameResult& Result)
{
	if (!ResultWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TTTPlayerController] ResultWidgetClass is null"));
		return;
	}

	if (!ResultWidgetInstance)
	{
		ResultWidgetInstance = CreateWidget<UUserWidget>(this, ResultWidgetClass);
	}

	if (ResultWidgetInstance && !ResultWidgetInstance->IsInViewport())
	{
		ResultWidgetInstance->AddToViewport();
	}
}
void ATTTPlayerController::OnResultRestartClicked()
{
	if (!IsLocalController())
	{
		return;
	}

	// 1) 결과창 닫기
	if (ResultWidgetInstance && ResultWidgetInstance->IsInViewport())
	{
		ResultWidgetInstance->RemoveFromParent();
	}

	// 2) GameInstance 결과 초기화
	if (UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>())
	{
		GI->ClearLastGameResult();
	}

	// 3) 로비 캐릭터 선택 UI 다시 열기
	OpenCharacterSelectUI();

	// 필요하면 Ready 상태 초기화
	if (ATTTPlayerState* PS = GetPlayerState<ATTTPlayerState>())
	{
		PS->ServerSetReady(false);
	}
}

void ATTTPlayerController::OnResultExitClicked()
{
	if (!IsLocalController())
	{
		return;
	}

	// 그냥 게임 종료
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, true);
}

void ATTTPlayerController::ServerRequestLobbyUIState_Implementation()
{
	// 서버에서만 의미 있음
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>())
	{
		if (GI->HasLastGameResult())
		{
			// 서버 GameInstance에 저장된 결과를 클라에게 보내서 결과창 띄우게 함
			const FTTTLastGameResult& Result = GI->GetLastGameResult();
			ClientShowLobbyResult(Result);
		}
		else
		{
			// 이번 판 결과가 없다 = 처음 로비 진입 → 캐릭터 선택 열기
			ClientShowLobbyCharacterSelect();
		}
	}
}

void ATTTPlayerController::ClientShowLobbyResult_Implementation(const FTTTLastGameResult& Result)
{
	// 클라에서 실제로 결과창 UI를 띄우는 함수
	ShowResultUI(Result);
}

void ATTTPlayerController::ClientShowLobbyCharacterSelect_Implementation()
{
	// 클라에서 캐릭터 선택 UI를 띄우는 함수
	OpenCharacterSelectUI();
}



