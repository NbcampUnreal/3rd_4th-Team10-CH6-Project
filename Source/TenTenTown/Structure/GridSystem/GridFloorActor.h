#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridFloorActor.generated.h"

class UBoxComponent;

UCLASS()
class TENTENTOWN_API AGridFloorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridFloorActor();

protected:
	virtual void BeginPlay() override;

	// 이 액터의 루트 컴포넌트이자, 라인 트레이스가 감지할 콜리전 영역입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
	TObjectPtr<UBoxComponent> GridBounds;

	// 그리드의 가로 / 세로 칸 수(에디터 변경 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = 1))
	int32 GridSizeX = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = 1))
	int32 GridSizeY = 3;

	// 한 칸(셀)의 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = 10.0))
	float CellSize = 300.f;

	// 각 그리드 셀의 점유 상태를 저장할 배열
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Grid State")
	TArray<bool> OccupancyGrid;

public:
	// public으로 프리뷰 액터가 접근
	FORCEINLINE float GetCellSize() const { return CellSize; }
	FORCEINLINE int32 GetGridSizeX() const { return GridSizeX; }
	FORCEINLINE int32 GetGridSizeY() const { return GridSizeY; }

	// 월드 좌표 변환
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool WorldToCellIndex(const FVector& WorldLocation, int32& OutX, int32& OutY) const;
	// 셀의 중앙 좌표 변환
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GetCellCenterWorldLocation(int32 X, int32 Y) const;
	// 셀이 그리드 안에 있는지 확인
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool IsValidCellIndex(int32 X, int32 Y) const;
};
