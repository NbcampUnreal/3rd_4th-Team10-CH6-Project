#include "Structure/GA/GA_InstallStructure.h"
#include "Structure/Data/StructureData.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "TTTGamePlayTags.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Engine/Engine.h"
#include "Structure/GridSystem/GridFloorActor.h"

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
		RowData->PreviewActorClass,
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
	if (!PreviewActor)
	{
		// 로컬 프리뷰가 없으면 그냥 취소
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 클라이언트의 로컬 PreviewActor에서 최종 위치와 회전값 가져옴
	const FVector FinalLocation = PreviewActor->GetActorLocation();
	const FRotator FinalRotation = PreviewActor->GetActorRotation();

	// 위치를 서버로 전송
	Server_RequestInstall(FinalLocation, FinalRotation);
}

// 취소 이벤트
void UGA_InstallStructure::OnCancel(FGameplayEventData Payload)
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	
	// 파괴
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_InstallStructure::Server_RequestInstall_Implementation(FVector Location, FRotator Rotation)
{
	// DT 가져오기
	FStructureData* RowData = nullptr;
	if (!StructureDataRow.IsNull())
	{
		RowData = StructureDataRow.GetRow<FStructureData>(TEXT("GA_InstallStructure::OnConfirm"));
	}

	// 데이터 검사
	if (!RowData || !RowData->ActualStructureClass)
	{
		bool bReplicateEndAbility = true;
		bool bWasCancelled = true;

		// 파괴 및 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	// --- [서버 측 검증 로직] ---
	AGridFloorActor* TargetGridFloor = nullptr;
	bool bIsValidInstallOnServer = false;
	const FVector PreviewLocation = Location;

	int32 CellX = -1;
	int32 CellY = -1;
		
	// 프리뷰 액터 위치에서 바닥을 확인
	FHitResult HitResult;
	FVector TraceStart = PreviewLocation + FVector(0, 0, 50.f); // 프리뷰 살짝 위
	FVector TraceEnd = PreviewLocation - FVector(0, 0, 50.f);   // 프리뷰 살짝 아래

	FCollisionQueryParams QueryParams;

	// 폰 아바타 무시
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		QueryParams.AddIgnoredActor(AvatarActor);
	}
	// 서버에 있는 프리뷰 액터도 무시
	if (PreviewActor)
	{
		QueryParams.AddIgnoredActor(PreviewActor);
	}

	// 트레이스 캐스팅
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel3, QueryParams))
	{
		TargetGridFloor = Cast<AGridFloorActor>(HitResult.GetActor());
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("SERVER: FAILED - Trace did not hit anything."));
	}
	
	// 캐스팅 성공 확인
	FVector SnappedCenterLocation = FVector::ZeroVector;
	
	if (TargetGridFloor)
	{
		bIsValidInstallOnServer = TargetGridFloor->WorldToCellIndex(PreviewLocation, CellX, CellY);

		if (bIsValidInstallOnServer)
		{
			// 중앙위치 스냅
			SnappedCenterLocation = TargetGridFloor->GetCellCenterWorldLocation(CellX, CellY);
		}
	}
	// --- [검증 로직 끝] ---

	// 스폰
	if (bIsValidInstallOnServer && PreviewLocation.Equals(SnappedCenterLocation, 1.0f))
	{
		const FVector FinalLocation = Location;
		const FRotator FinalRotation = Rotation;
			
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

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			
		// (TODO: TargetGridFloor->OccupyCell(CellX, CellY) 등 점유 로직 추가)
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("SERVER: FAILED - bIsValidInstallOnServer is false. Cancelling."));EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
