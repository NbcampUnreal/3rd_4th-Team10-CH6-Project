#include "Structure/GridSystem/GridFloorActor.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"

AGridFloorActor::AGridFloorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 루트 설정
	GridBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("GridBounds"));
	SetRootComponent(GridBounds);
	
	// 오브젝트 타입을 커스텀 오브젝트 타입(ECC_GameTraceChannel3)으로 설정
	GridBounds->SetCollisionObjectType(ECC_GameTraceChannel3); 
	// 콜리전은 쿼리(라인 트레이스)에만 사용, 물리 X
	GridBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// 다른 채널과의 반응 무시
	GridBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	// GridFloor(ECC_GameTraceChannel3)에 대해서만 Block으로 반응
	GridBounds->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
}

void AGridFloorActor::BeginPlay()
{
	Super::BeginPlay();

	OccupancyGrid.Init(false, GridSizeX * GridSizeY);

	// 에디터에서 박스 크기를 그리드 크기에 맞게 자동으로 조절해 주는 로직
	if (GridBounds)
	{
		const FVector BoxExtent = FVector(
			(GridSizeX * CellSize) * 0.5f,
			(GridSizeY * CellSize) * 0.5f,
			10.0f 
		);
		GridBounds->SetBoxExtent(BoxExtent);
		GridBounds->SetRelativeLocation(FVector(BoxExtent.X, BoxExtent.Y, -10.0f));
	}
}

bool AGridFloorActor::WorldToCellIndex(const FVector& WorldLocation, int32& OutX, int32& OutY) const
{
	// 그리드 액터의 원점(ActorLocation)을 기준으로 계산
	const FVector RelativeLocation = WorldLocation - GetActorLocation();

	// 상대 위치를 CellSize로 나누어 계산
	OutX = FMath::FloorToInt(RelativeLocation.X / CellSize);
	OutY = FMath::FloorToInt(RelativeLocation.Y / CellSize);

	// 유효한 범위인지 확인
	return IsValidCellIndex(OutX, OutY);
}

FVector AGridFloorActor::GetCellCenterWorldLocation(int32 X, int32 Y) const
{
	// 셀의 중앙 계산(설치용)
	const float CenterOffsetX = (X * CellSize) + (CellSize * 0.5f);
	const float CenterOffsetY = (Y * CellSize) + (CellSize * 0.5f);

	// 위치 값 반환
	return GetActorLocation() + FVector(CenterOffsetX, CenterOffsetY, 0.f);
}

bool AGridFloorActor::IsValidCellIndex(int32 X, int32 Y) const
{
	// X가 0보다 크거나 같고, GridSizeX보다 작은지 확인
	// Y가 0보다 크거나 같고, GridSizeY보다 작은지 확인
	return (X >= 0 && X < GridSizeX && Y >= 0 && Y < GridSizeY);
}

