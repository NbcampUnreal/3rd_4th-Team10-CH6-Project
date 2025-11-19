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
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StructureList, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ItemList, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bIsReady, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME(ATTTPlayerState, SelectedCharacterClass);
}

void ATTTPlayerState::OnRep_Gold()
{
	OnGoldChangedDelegate.Broadcast(Gold);
}

void ATTTPlayerState::OnRep_InventoryStructure()
{
	OnStructureListChangedDelegate.Broadcast();
}
void ATTTPlayerState::OnRep_InventoryItem()
{
	OnItemListChangedDelegate.Broadcast();
}

void ATTTPlayerState::AddGold(int32 Amount)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Gold += Amount;
		ForceNetUpdate();
	}
}


bool ATTTPlayerState::Server_AddGold_Validate(int32 Amount)
{
	//음수로 차감 시 골드가 0 미만으로 떨어지지 않는지 확인
	if (Amount < 0 && Gold + Amount < 0)
	{
		return false;
	}
	return true;
}
void ATTTPlayerState::Server_AddGold_Implementation(int32 Amount)
{
	Gold += Amount;
	ForceNetUpdate();
}
void ATTTPlayerState::OnRep_IsReady()
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerState] Ready changed: %s (IsReady=%d)"),
	   *GetPlayerName(), bIsReady ? 1 : 0);
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
void ATTTPlayerState::Server_UpdateStructureData_Implementation(const FInventoryItemData& NewStructureData)
{
	//서버에서 데이터 찾기
	FInventoryItemData* ItemData = FindStructureDataByName(NewStructureData.ItemName);

	if (ItemData)
	{
		//서버 데이터 업데이트
		ItemData->Count = NewStructureData.Count;
		ItemData->Level = NewStructureData.Level;

		MARK_PROPERTY_DIRTY_FROM_NAME(ATTTPlayerState, StructureList, this);

		//강제복제
		ForceNetUpdate();
	}
}
void ATTTPlayerState::Server_UpdateItemData_Implementation(const FInventoryItemData& NewItemData)
{
	//서버에서 데이터 찾기
	FInventoryItemData* ItemData = FindItemDataByName(NewItemData.ItemName);

	if (ItemData)
	{
		//서버 데이터 업데이트
		ItemData->Count = NewItemData.Count;
		ItemData->Level = NewItemData.Level;

		MARK_PROPERTY_DIRTY_FROM_NAME(ATTTPlayerState, ItemList, this);

		//강제복제
		ForceNetUpdate();
	}
}


FInventoryItemData* ATTTPlayerState::FindStructureDataByName(const FText& FindItemName)
{
	FInventoryItemData* FoundItem = StructureList.FindByPredicate(
		[&FindItemName](const FInventoryItemData& Item)
		{
			return FindItemName.EqualTo(Item.ItemName);
		}
	);

	return FoundItem;
}

FInventoryItemData* ATTTPlayerState::FindItemDataByName(const FText& FindItemName)
{
	FInventoryItemData* FoundItem = ItemList.FindByPredicate(
		[&FindItemName](const FInventoryItemData& Item)
		{
			return FindItemName.EqualTo(Item.ItemName);
		}
	);
	return FoundItem;
}
void ATTTPlayerState::OnRep_SelectedCharacterClass()
{
	UE_LOG(LogTemp, Warning, TEXT("[OnRep_SelectedCharacterClass] Player=%s  SelectedClass=%s"),
		*GetPlayerName(), *GetNameSafe(SelectedCharacterClass));
}
