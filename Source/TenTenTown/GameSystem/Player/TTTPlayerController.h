// TTTPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "TTTPlayerController.generated.h"


UCLASS()
class TENTENTOWN_API ATTTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATTTPlayerController();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TTT|UI")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget* HUDInstance = nullptr;
	
};
