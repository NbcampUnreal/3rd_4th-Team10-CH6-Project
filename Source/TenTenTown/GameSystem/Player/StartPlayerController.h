// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StartPlayerController.generated.h"


class UUserWidget;

UCLASS()
class TENTENTOWN_API AStartPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AStartPlayerController();

	virtual void BeginPlay() override;

	/** [클라] Apply 버튼: IP/Port 저장만 */
	UFUNCTION(BlueprintCallable, Category="StartMenu")
	void ApplyConnectInfo(const FString& InIP, int32 InPort);

	/** [클라] Play 버튼: 저장된 IP/Port로 서버 접속 */
	UFUNCTION(BlueprintCallable, Category="StartMenu")
	void PlayWithSavedConnectInfo();

protected:

	/** 시작 메뉴 위젯 클래스 (WBP_StartMenuTest 할당) */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> StartMenuClass;

private:
	/** 생성된 시작 메뉴 인스턴스 */
	UPROPERTY()
	TObjectPtr<UUserWidget> StartMenuInstance;
	
};
