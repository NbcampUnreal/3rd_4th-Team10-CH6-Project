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
	// 서버 호스트: 포트 미지정 시 -1(기본 포트 사용)
	UFUNCTION(Exec)
	void HostLobbyCmd(int32 Port = -1);

	// 클라이언트 조인: IP 필수, 포트 미지정 시 -1(기본 포트 사용)
	UFUNCTION(Exec)
	void JoinLobbyCmd(const FString& IP, int32 Port = -1);

	// 현재 유효 포트 확인 (디버그)
	UFUNCTION(Exec)
	void ShowEffectivePort(int32 Port = -1);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TTT|UI")
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget* HUDInstance = nullptr;
	
};
