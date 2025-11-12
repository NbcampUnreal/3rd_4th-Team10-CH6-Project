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
	bReplicates = false;
	bNetLoadOnClient = false;
	bOnlyRelevantToOwner = true;
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
		
		if (HitGridFloor) // 성공 시
		{
			int32 CellX, CellY;
			bool bIsValidCell = HitGridFloor->WorldToCellIndex(HitResult.Location, CellX, CellY);

			if (bIsValidCell)
			{
				FVector SnappedLocation = HitGridFloor->GetCellCenterWorldLocation(CellX, CellY);
				SetActorLocation(SnappedLocation);
				DebugMsg = FString::Printf(TEXT("SUCCESS: Snapped to Cell (%d, %d)"), CellX, CellY);

				OnInstallStatusChanged(true);
			}
			else
			{
				SetActorLocation(HitResult.TraceEnd);
				DebugMsg = TEXT("FAIL: Hit GridFloor but outside valid cell range.");

				OnInstallStatusChanged(false);
			}
		}
		else // 실패 시
		{
			SetActorLocation(HitResult.TraceEnd);
			DebugMsg = FString::Printf(TEXT("FAIL: Hit Actor is NOT GridFloor. Hit: %s"), *HitResult.GetActor()->GetName());

			OnInstallStatusChanged(false);
		}
	}
	else
	{
		SetActorLocation(HitResult.TraceEnd);
		DebugMsg = TEXT("FAIL: Trace did not hit anything.");

		OnInstallStatusChanged(false);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, DebugMsg);
	}
}
