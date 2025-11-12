#include "Structure/GA/GA_InstallStructure.h"
#include "Structure/Data/StructureData.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "TTTGamePlayTags.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"

// 태그 부여
UGA_InstallStructure::UGA_InstallStructure()
{
	ActivationOwnedTags.AddTag(GASTAG::State_IsInstall);
}

void UGA_InstallStructure::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 데이터 테이블 정보 읽어오기
	FStructureData* RowData = nullptr;
	if (!StructureDataRow.IsNull())
	{
		RowData = StructureDataRow.GetRow<FStructureData>(TEXT("GA_InstallStructure::ActivateAbility"));
	}

	if (!RowData || !RowData->PreviewActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_InstallStructure: 데이터 테이블 행이 잘못되었거나 PreviewActorClass가 없습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true); // DT 없으면 취소
		return;
	}

	// 프리뷰 액터 스폰
	AActor* OwningActor = ActorInfo->OwnerActor.Get();
	if (!OwningActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true); // 주인이 없으면 취소
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwningActor;
	SpawnParams.Instigator = Cast<APawn>(ActorInfo->AvatarActor.Get());

	// 일단 캐릭터 위치에 스폰(다음은 틱이 옮김)
	PreviewActor = GetWorld()->SpawnActor<AActor>(
		RowData->PreviewActorClass, // DT의 PreviewActorClass 가져옴
		OwningActor->GetActorLocation(), 
		OwningActor->GetActorRotation(),
		SpawnParams
	);
	
	if (!PreviewActor)
	{
		UE_LOG(LogTemp, Error, TEXT("GA_InstallStructure: 프리뷰 액터 스폰에 실패했습니다."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 확정 시 이벤트
	UAbilityTask_WaitGameplayEvent* ConfirmTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GASTAG::State_Structure_Confirm);
	ConfirmTask->EventReceived.AddDynamic(this, &UGA_InstallStructure::OnConfirm);
	ConfirmTask->ReadyForActivation();

	// 취소 시 이벤트
	UAbilityTask_WaitGameplayEvent* CancelTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GASTAG::State_Structure_Cancel);
	CancelTask->EventReceived.AddDynamic(this, &UGA_InstallStructure::OnCancel);
	CancelTask->ReadyForActivation();
}

// 어빌리티 종료(확정, 취소, 사망 등)
void UGA_InstallStructure::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


// 확정 이벤트
void UGA_InstallStructure::OnConfirm(FGameplayEventData Payload)
{
	// DT 가져오기
	FStructureData* RowData = nullptr;
	if (!StructureDataRow.IsNull())
	{
		RowData = StructureDataRow.GetRow<FStructureData>(TEXT("GA_InstallStructure::OnConfirm"));
	}

	// 데이터 검사
	if (!RowData || !RowData->ActualStructureClass || !PreviewActor)
	{
		bool bReplicateEndAbility = true;
		bool bWasCancelled = true;

		// 파괴 및 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	// 서버에서 진행
	if (GetAbilitySystemComponentFromActorInfo()->IsOwnerActorAuthoritative())
	{
		const FVector FinalLocation = PreviewActor->GetActorLocation();
		const FRotator FinalRotation = PreviewActor->GetActorRotation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwningActorFromActorInfo();
		SpawnParams.Instigator = GetOwningActorFromActorInfo()->GetInstigator();

		// 스폰
		GetWorld()->SpawnActor<AActor>(
			RowData->ActualStructureClass, 
			FinalLocation, 
			FinalRotation, 
			SpawnParams
		);
	}
	
	// 종료 설정
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// 취소 이벤트
void UGA_InstallStructure::OnCancel(FGameplayEventData Payload)
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	
	// 파괴
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}