// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Character/Characters/Fighter/FighterCharacter.h"

ATTTPlayerState::ATTTPlayerState()
{
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(ReplicationMode);

	Gold=0;
}
