// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/MVVM/LobbyViewModel.h"
#include "ResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UListView* ResultListView;

	UPROPERTY(BlueprintReadOnly, Category = "MVVM")
	TObjectPtr<ULobbyViewModel> LobbyViewModel;

public:
	void SetLobbyViewModel(ULobbyViewModel* InViewModel);
	void SetResultListView();


	
};
