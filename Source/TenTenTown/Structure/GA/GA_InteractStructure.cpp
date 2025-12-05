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
		int32 Cost = TargetStructure->GetUpgradeCost();
		
		// 비용 차감
		if (CheckCostAndDeduct(Cost))
		{
			TargetStructure->UpgradeStructure();
			UE_LOG(LogTemp, Log, TEXT("Upgrade Success! (Free for now)"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Upgrade Failed (Should not happen with 0 cost)"));
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
