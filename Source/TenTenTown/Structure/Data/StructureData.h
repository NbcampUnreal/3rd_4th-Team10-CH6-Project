#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h" // 추가 예정
#include "StructureData.generated.h"

USTRUCT(BlueprintType)
struct FStructureLevelInfo
{
	GENERATED_BODY()
public:
	// 이 레벨에서 사용할 메쉬 (회전체 부분)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UStaticMesh> TurretMesh; 
    
	// 다음 레벨로 업그레이드하는 데 필요한 비용 (1->2 비용, 2->3 비용...)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 UpgradeCost = 0; // *주의: 마지막 레벨(3단계)의 비용은 0이 됩니다.
    
	// [GAS] 이 레벨을 달성했을 때 적용할 GameplayEffect 클래스 (스탯 갱신)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<UGameplayEffect> LevelStatGE; 

	// [GAS] 이 레벨에서 새로 습득하는 Ability (예: 3단계의 특별 효과)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> NewAbilities;
};

USTRUCT(BlueprintType)
struct FStructureData: public FTableRowBase
{
	GENERATED_BODY()

public:
	// [UI] 구조물 이름(한글 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText DisplayName;
	// [UI] 구조물 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText Description;
	// [UI] 구조물 이미지(TSoftObjectPtr = 실제 필요할 때 로딩)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSoftObjectPtr<UTexture2D> StructureImage;
	// [UI, 인게임] 최대 설치 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI | Ingame")
	int32 MaxInstallCount = 10;
	// [UI, 인게임] 최대 업글 단계
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI | Ingame")
	int32 MaxUpgradeLevel = 3;
	// [인게임] 고정 베이스 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame")
	TSoftObjectPtr<UStaticMesh> BaseMesh;
	// [인게임] 레벨별 정보 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ingame")
	TArray<FStructureLevelInfo> LevelInfos;
	// [인게임] 프리뷰 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingame")
	TSubclassOf<AActor> PreviewActorClass;
	// [인게임] 실제 스폰 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingame")
	TSubclassOf<AActor> ActualStructureClass;
};
