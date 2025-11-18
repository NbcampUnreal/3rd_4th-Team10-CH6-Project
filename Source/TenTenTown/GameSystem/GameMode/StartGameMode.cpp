// StartGameMode.cpp

#include "GameSystem/GameMode/StartGameMode.h"   // 실제 경로에 맞게 수정
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void AStartGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!StartMenuClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StartGameMode] StartMenuClass is not set."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StartGameMode] World is null."));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StartGameMode] PlayerController not found."));
		return;
	}

	if (!StartMenuInstance)
	{
		StartMenuInstance = CreateWidget<UUserWidget>(PC, StartMenuClass);
		if (StartMenuInstance)
		{
			StartMenuInstance->AddToViewport();

			// 마우스 커서 + UI Only 입력 모드 설정
			PC->bShowMouseCursor = true;

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(StartMenuInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);

			UE_LOG(LogTemp, Log, TEXT("[StartGameMode] StartMenu widget created and added to viewport."));
		}
	}
}
