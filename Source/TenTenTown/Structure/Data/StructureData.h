#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "StructureData.generated.h"

USTRUCT(BlueprintType)
struct FStructureData: public FTableRowBase
{
	GENERATED_BODY()

	// 프리뷰 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data")
	TSubclassOf<AActor> PreviewActorClass;

	// 실제 스폰 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data")
	TSubclassOf<AActor> ActualStructureClass;

	// 3. 설치 비용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data")
	float InstallCost = 0.f;
};
