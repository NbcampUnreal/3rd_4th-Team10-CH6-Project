#include "Structure/Preview/BasePreviewActor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/GridSystem/GridFloorActor.h"

ABasePreviewActor::ABasePreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetActorEnableCollision(false);
}

void ABasePreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	// ... (뷰포트 및 월드 좌표 변환 코드) ...
	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	const float ScreenCenterX = ViewportSizeX * 0.5f;
	const float ScreenCenterY = ViewportSizeY * 0.5f;

	FVector WorldLocation;
	FVector WorldDirection;
	if (!PC->DeprojectScreenPositionToWorld(ScreenCenterX, ScreenCenterY, WorldLocation, WorldDirection))
	{
		return;
	}

	FVector TraceStart = WorldLocation;
	FVector TraceEnd = TraceStart + (WorldDirection * MaxInstallDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (GetInstigator())
	{
		QueryParams.AddIgnoredActor(GetInstigator());
	}
	
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 0.1f, 0, 1.f);
	
	FString DebugMsg = TEXT("DEFAULT: No Error");

	// [복원] '이전 방식'인 LineTraceSingleByChannel 사용
	// BasePreviewActor.h에서 TraceChannel이 ECC_GameTraceChannel3로 설정되었습니다.
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		TraceChannel, // ECC_GameTraceChannel3
		QueryParams
	);

	if (bHit)
	{
		AGridFloorActor* HitGridFloor = Cast<AGridFloorActor>(HitResult.GetActor());
		
		if (HitGridFloor) // [성공] BP_GridFloor를 맞췄을 때
		{
			int32 CellX, CellY;
			bool bIsValidCell = HitGridFloor->WorldToCellIndex(HitResult.Location, CellX, CellY);

			if (bIsValidCell)
			{
				FVector SnappedLocation = HitGridFloor->GetCellCenterWorldLocation(CellX, CellY);
				SetActorLocation(SnappedLocation);
				DebugMsg = FString::Printf(TEXT("SUCCESS: Snapped to Cell (%d, %d)"), CellX, CellY);
			}
			else
			{
				SetActorLocation(HitResult.TraceEnd);
				DebugMsg = TEXT("FAIL: Hit GridFloor but outside valid cell range.");
			}
		}
		else // [문제] '일반 바닥' 등을 맞췄을 때
		{
			SetActorLocation(HitResult.TraceEnd);
			DebugMsg = FString::Printf(TEXT("FAIL: Hit Actor is NOT GridFloor. Hit: %s"), *HitResult.GetActor()->GetName());
		}
	}
	else
	{
		SetActorLocation(HitResult.TraceEnd);
		DebugMsg = TEXT("FAIL: Trace did not hit anything.");
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, DebugMsg);
	}
}