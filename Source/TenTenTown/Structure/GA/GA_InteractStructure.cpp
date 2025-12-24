#include "Structure/GA/GA_InteractStructure.h"
#include "Structure/Base/StructureBase.h"
#include "Character/PS/TTTPlayerState.h"
#include "TTTGamePlayTags.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Structure/GridSystem/GridFloorActor.h"
#include "Kismet/GameplayStatics.h"

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
		
		// --- 그리드 점유 해제 로직 ---
		// 구조물의 현재 위치 가져오기
		FVector StructureLocation = TargetStructure->GetActorLocation();

		// 그리드 액터 찾기
		FHitResult HitResult;
		FVector TraceStart = StructureLocation + FVector(0, 0, 100.f);
		FVector TraceEnd = StructureLocation - FVector(0, 0, 100.f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(TargetStructure);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel3, QueryParams))
		{
			AGridFloorActor* GridActor = Cast<AGridFloorActor>(HitResult.GetActor());
			if (GridActor)
			{
				// 점유 해제 요청
				GridActor->TryRemoveStructure(StructureLocation);
				UE_LOG(LogTemp, Log, TEXT("GA_Interact: Cell Freed at %s"), *StructureLocation.ToString());
			}
		}
		// --- 끝 ---

		AddGold(ReturnGold); // 골드 반환
		TargetStructure->SellStructure(); // 구조물 파괴
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
	ATTTPlayerState* PS = nullptr;

	// 1. 어빌리티의 소유자 액터(Owner Actor)를 가져옵니다.
	AActor* OwnerActor = GetOwningActorFromActorInfo();

	// 2. 소유자가 폰(캐릭터)이라면, 엔진 내장 함수인 GetPlayerState()를 사용합니다.
	// (ActorInfo 구조체를 직접 건드리지 않아서 컴파일 오류가 사라집니다.)
	if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		PS = Cast<ATTTPlayerState>(OwnerPawn->GetPlayerState());
	}
	// 3. 만약 소유자가 PlayerState 그 자체라면 (혹시 모를 상황 대비)
	else
	{
		PS = Cast<ATTTPlayerState>(OwnerActor);
	}

	// 4. 골드 지급 로직
	if (PS)
	{
		PS->Server_AddGold(Amount);
		UE_LOG(LogTemp, Log, TEXT("[Interact] Refunded Gold: %d"), Amount);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Interact] PlayerState NOT FOUND. Cannot refund gold."));
	}
}
