#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
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

	// UI용
	// 설치 비용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data")
	float InstallCost;
	// 이미지 (UI용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data | UI")
	TSoftObjectPtr<UTexture2D> StructureImage;
	// 최대 설치 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data | UI")
	int32 MaxInstallCount;
	// 단계별 업그레이드 비용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data | UI")
	TArray<float> UpgradeCosts;
	// 최대 업글 단계
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data | UI")
	int32 MaxUpgradeLevel;
	// 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data | UI")
	FText StructureName;
	// 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Structure Data | UI")
	FText Description;

	FStructureData() : InstallCost(0.f), MaxInstallCount(10), MaxUpgradeLevel(3)
	{
		UpgradeCosts.Init(0.f, 2);
	}
};
