#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "StructureUpgradeData.generated.h"

USTRUCT(BlueprintType)
struct FStructureUpgradeData
{
	GENERATED_BODY()

public:
	// 이 레벨에서 사용할 메쉬
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMesh> StructureMesh;

	// (선택) 발사체나 터렛 회전부 등 추가 메쉬가 있다면 추가

	// 업그레이드 비용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	float UpgradeCost = 0;
	
	// 이 레벨이 될 때 적용할 스탯 변경 이펙트 (예: 공격력 증가, 체력 증가)
	// GE_UpgradeStat_Level2 같은 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> LevelStatEffect;

	// (선택) 이 레벨에서 새로 생기는 스킬 (예: 3레벨부터 멀티샷 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> NewAbilities;
};
