// StartGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StartGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API AStartGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

protected:
	// 시작 메뉴 위젯 클래스 (WBP_StartMenuTest 할당)
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> StartMenuClass;

private:
	// 생성된 위젯 인스턴스
	UPROPERTY()
	UUserWidget* StartMenuInstance;
};
