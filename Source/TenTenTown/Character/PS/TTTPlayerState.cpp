// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Character/Characters/Fighter/FighterCharacter.h"
#include "Net/UnrealNetwork.h"

ATTTPlayerState::ATTTPlayerState()
{
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(ReplicationMode);

	Gold=0;
}

void ATTTPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass,Gold,COND_None,REPNOTIFY_OnChanged);
}

void ATTTPlayerState::OnRep_Gold()
{
	//여기서 ui 갱신하기
}
