#include "Structure/Preview/BasePreviewActor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ABasePreviewActor::ABasePreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// 콜리전 끄기
	SetActorEnableCollision(false);
}

void ABasePreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// 2. 이 컨트롤러가 존재하고, '로컬 컨트롤러'(서버나 다른 클라이언트가 아닌)인지 확인합니다.
	if (!PC || !PC->IsLocalController())
	{
		// 이 코드가 서버에서 실행 중이거나,
		// 이 프리뷰를 조종할 수 없는 다른 클라이언트에서 실행 중이라면,
		// Tick 로직을 즉시 중단합니다.
		return;
	}

	// 1. 뷰포트(화면) 크기를 가져옵니다.
	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// 2. 화면의 정중앙 좌표를 계산합니다.
	const float ScreenCenterX = ViewportSizeX * 0.5f;
	const float ScreenCenterY = ViewportSizeY * 0.5f;

	// 3. 2D 화면 중앙 좌표를 3D 월드의 '위치'와 '방향'으로 변환(Deproject)합니다.
	FVector WorldLocation; // 트레이스의 시작점이 될 위치
	FVector WorldDirection; // 트레이스가 나아갈 방향
	
	if (!PC->DeprojectScreenPositionToWorld(ScreenCenterX, ScreenCenterY, WorldLocation, WorldDirection))
	{
		// 변환에 실패하면 아무것도 하지 않습니다.
		return;
	}

	// 4. 라인 트레이스 시작점과 끝점을 설정합니다.
	// WorldLocation은 보통 카메라 위치이므로 여기서부터 시작합니다.
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
	
	// 라인트레이스
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		TraceChannel, // 콜리전 채널(바닥)
		QueryParams
	);

	// 액터 위치 이동
	if (bHit)
	{
		// 맞은 곳
		SetActorLocation(HitResult.Location);
	}
	else
	{
		// 최대 거리
		SetActorLocation(HitResult.TraceEnd);
	}
}

