// Fill out your copyright notice in the Description page of Project Settings.


#include "TTTPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Character/Characters/Fighter/FighterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameSystem/GameMode/LobbyGameMode.h"
#include "Engine/World.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"

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
	DOREPLIFETIME(ThisClass,KillCount);
}

void ATTTPlayerState::OnRep_Gold()
{
	//OnGoldChangedDelegate.Broadcast(Gold);
}

void ATTTPlayerState::OnRep_KillCount()
{
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

void ATTTPlayerState::OnRep_SelectedCharacterClass()
{
	UE_LOG(LogTemp, Warning, TEXT("[OnRep_SelectedCharacterClass] Player=%s  SelectedClass=%s"),
		*GetPlayerName(), *GetNameSafe(SelectedCharacterClass));
}

int32 ATTTPlayerState::GetKillcount()
{
	return KillCount;
}

void ATTTPlayerState::SetKillcount(int32 NewKillcount)
{
	KillCount = NewKillcount;
}

void ATTTPlayerState::SetKillcountZero()
{
	KillCount = 0;
}

void ATTTPlayerState::AddKillcount(int32 Plus)
{
	KillCount +=Plus;
}


#pragma region UI_Region
//void ATTTPlayerState::InitializeStructureList(const TArray<FInventoryItemData>& InitialList)
//{
//	//서버 권한이 있는지 다시 한번 확인합니다. (방어적 코드)
//	if (HasAuthority())
//	{
//		// 1. StructureList에 초기 데이터를 설정합니다.
//		StructureList = InitialList;
//
//		// 2. 복제 시스템이 이 변경 사항을 클라이언트에게 보냅니다.
//		// (클라이언트는 OnRep_InventoryStructure()를 호출하며, 
//		// 이 함수가 다시 OnStructureListChangedDelegate를 브로드캐스트하여 MVVM 뷰모델을 깨웁니다.)
//
//		// 3. 서버 측에서 즉시 델리게이트 호출이 필요한 경우 (선택적)
//		// OnStructureListChangedDelegate.Broadcast(); 
//	}
//}

void ATTTPlayerState::OnAbilitySystemInitialized()
{
	UE_LOG(LogTemp, Log, TEXT("[PlayerState] Ability System Initialized for Player: %s"), *GetPlayerName());
	
	if (!HasAuthority())
	{
		Server_NotifyReady();
		return;
	}
	
	ATTTGameStateBase* GS = Cast<ATTTGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->NotifyPlayerReady();
	}
	
	
}
void ATTTPlayerState::Server_NotifyReady_Implementation()
{
	ATTTGameStateBase* GS = Cast<ATTTGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->NotifyPlayerReady();
	}
}
#pragma endregion

