// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Character/Characters/Fighter/FighterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameSystem/GameMode/LobbyGameMode.h"
#include "Engine/World.h"

ATTTPlayerState::ATTTPlayerState()
{
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(ReplicationMode);
	Gold=0;

	Gold = 0;
	bIsReady = false;
	SelectedCharacterClass = nullptr;
}

void ATTTPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass,Gold,COND_None,REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bIsReady, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME(ATTTPlayerState, SelectedCharacterClass);
}

void ATTTPlayerState::OnRep_Gold()
{
	//여기서 ui 갱신하기
}
void ATTTPlayerState::OnRep_IsReady()
{
	// 여기서 Ready 상태 UI 갱신 (예: 준비완료 텍스트 색 변경 등)
}

void ATTTPlayerState::ToggleReady()
{
	ServerSetReady(!bIsReady);
}

void ATTTPlayerState::ServerSetReady_Implementation(bool bNewReady)
{
	if (bIsReady == bNewReady)
	{
		return;
	}

	bIsReady = bNewReady;

	// LobbyGameMode에 "Ready 바뀌었다" 통보
	if (UWorld* World = GetWorld())
	{
		if (AGameModeBase* GMBase = World->GetAuthGameMode())
		{
			if (ALobbyGameMode* LobbyGM = Cast<ALobbyGameMode>(GMBase))
			{
				LobbyGM->HandlePlayerReadyChanged(this);
			}
		}
	}
}