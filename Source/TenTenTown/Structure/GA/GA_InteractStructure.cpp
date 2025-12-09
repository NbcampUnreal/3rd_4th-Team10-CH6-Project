#include "Structure/GA/GA_InteractStructure.h"
#include "Structure/Base/StructureBase.h"
#include "Character/PS/TTTPlayerState.h"
#include "TTTGamePlayTags.h"

UGA_InteractStructure::UGA_InteractStructure()
{
	// 상호작용은 서버에서
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly; 
}

void UGA_InteractStructure::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 데이터 확인
	if (!TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 구조물 확인
	AActor* TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	AStructureBase* TargetStructure = Cast<AStructureBase>(TargetActor);
	
	if (!TargetStructure)
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_InteractStructure: Target is NULL or Not StructureBase"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FGameplayTag EventTag = TriggerEventData->EventTag;

	// [G키] 업그레이드
	if (EventTag == GASTAG::Event_Build_Upgrade)
	{
		int32 NextLevel = TargetStructure->CurrentUpgradeLevel + 1;
		int32 Cost = 0;

		// 레벨 정보를 가져오는 함수가 TargetStructure 내에 구현되어야 합니다.
		// 임시로 직접 데이터 테이블을 참조하는 로직을 가정합니다.
		
		if (TargetStructure->CurrentUpgradeLevel < TargetStructure->CachedStructureData.MaxUpgradeLevel)
		{
			// 다음 레벨의 인덱스 = 현재 레벨 (CurrentUpgradeLevel이 1부터 시작)
			const FStructureLevelInfo& NextLevelInfo = TargetStructure->CachedStructureData.LevelInfos[NextLevel - 1];
			Cost = NextLevelInfo.UpgradeCost;
		}

		if (Cost > 0)
		{
			// 비용 차감
			if (CheckCostAndDeduct(Cost))
			{
				TargetStructure->UpgradeStructure();
				UE_LOG(LogTemp, Log, TEXT("Upgrade Success! Cost: %d"), Cost);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Upgrade Failed (Not Enough Gold)"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Upgrade Failed (Max Level or Cost is 0)"));
		}
	}
	// [H키] 판매
	else if (EventTag == GASTAG::Event_Build_Sell)
	{
		int32 ReturnGold = TargetStructure->GetSellReturnAmount();
		
		AddGold(ReturnGold); // 골드 반환
		TargetStructure->SellStructure();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool UGA_InteractStructure::CheckCostAndDeduct(int32 Cost)
{
	UE_LOG(LogTemp, Log, TEXT("[Cost Check] Need: %d | Bypassing Gold Check (Always True)"), Cost);
	return true; 

	/* -- 나중에 골드 시스템 구현 시 주석 해제 --
	ATTTPlayerState* PS = Cast<ATTTPlayerState>(GetActorInfo().PlayerState.Get());
	if (PS)
	{
		if (PS->GetGold() >= Cost)
		{
			PS->Server_AddGold(-Cost);
			return true;
		}
	}
	return false;
	*/
}

void UGA_InteractStructure::AddGold(int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("[Add Gold] Amount: %d | System Not Implemented"), Amount);

	/* -- 나중에 구현 시 주석 해제 --
	ATTTPlayerState* PS = Cast<ATTTPlayerState>(GetActorInfo().PlayerState.Get());
	if (PS)
	{
		PS->Server_AddGold(Amount);
	}
	*/
}
