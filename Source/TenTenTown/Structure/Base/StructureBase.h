#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Structure/Data/AS/AS_StructureAttributeSet.h"
#include "Structure/Data/StructureData.h"
#include "StructureBase.generated.h"

UCLASS()
class TENTENTOWN_API AStructureBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	AStructureBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

	// [GAS] 공통 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAS_StructureAttributeSet> AttributeSet;
	
public:
	// --- 데이터 테이블 정보 (자식에서 설정) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* StructureDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName StructureRowName;

	// 캐싱된 데이터 (매번 테이블 찾지 않도록 저장)
	FStructureData CachedStructureData;

	// --- 상태 변수 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	int32 CurrentUpgradeLevel = 1;

	// 초기화 (설치 시 호출)
	virtual void InitializeStructure();

	// 업그레이드 비용 반환 (데이터 테이블 기반)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual int32 GetUpgradeCost() const;
	// 판매 반환금 반환 (설치비 + 업글비의 일부)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual int32 GetSellReturnAmount() const;
	// 업그레이드 실행
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void UpgradeStructure();
	// 판매 실행
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void SellStructure();

};
