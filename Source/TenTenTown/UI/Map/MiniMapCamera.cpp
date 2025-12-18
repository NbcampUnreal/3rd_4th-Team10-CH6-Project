#include "UI/Map/MiniMapCamera.h"


AMiniMapCamera::AMiniMapCamera()
{
	PrimaryActorTick.bCanEverTick = false;

	MiniMapCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCapture"));
	RootComponent = MiniMapCaptureComponent;

	MiniMapCaptureComponent->OrthoWidth = OrthoWidth;

	MiniMapCaptureComponent->bCaptureEveryFrame = false;
	MiniMapCaptureComponent->bAlwaysPersistRenderingState = true;
}


void AMiniMapCamera::BeginPlay()
{
	//Super::BeginPlay();

	if (MiniMapRenderTarget)
	{
		SetActorLocation(MiniMapLocation);
		SetActorRotation(MiniMapRotation);
	
		MiniMapCaptureComponent->OrthoWidth = OrthoWidth;

		MiniMapCaptureComponent->TextureTarget = MiniMapRenderTarget;

		MiniMapCaptureComponent->CaptureScene();
	}
}



FVector2D AMiniMapCamera::GetPlayerMiniMapPosition(FVector PlayerLocation)
{
	const FVector MapCenter = GetActorLocation();

	const float MapSize = MiniMapCaptureComponent->OrthoWidth;
		
	if (MapSize <= 0.f) return FVector2D(0.5f, 0.5f);

	float MapLeftBound = MapCenter.Y - (MapSize * 0.5f);
	float MapTopBound = MapCenter.X + (MapSize * 0.5f);

	float Ratio_X = (PlayerLocation.Y - MapLeftBound) / MapSize;
	float Ratio_Y = (MapTopBound - PlayerLocation.X) / MapSize;

	FVector CenterOffset = MapCenter;
	


	return FVector2D(
		FMath::Clamp(Ratio_X, 0.f, 1.f),
		FMath::Clamp(Ratio_Y, 0.f, 1.f)
	);

	
}
