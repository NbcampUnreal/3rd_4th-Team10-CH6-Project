#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structure/Crossbow/CrossbowBolt.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Structure/Data/AS/AS_StructureAttributeSet.h"
#include "Structure/Data/StructureData.h"
#include "CrossbowStructure.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UAbilitySystemComponent;
class UStructureAttributeSet;

UCLASS()
class TENTENTOWN_API ACrossbowStructure : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	ACrossbowStructure();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
public:	
	// 받침대
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMesh;
	// 몸체(회전)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TurretMesh;
	// 발사 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzleLocation;
	// 사거리 감지용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DetectSphere;

	// ASC
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	UAS_StructureAttributeSet* AttributeSet;
	
	// 데이터 테이블 가져오기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* StructureDataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FName StructureRowName;
	UFUNCTION(BlueprintCallable, Category = "Data")
	void RefreshStatus();

	// --- 스탯 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackRange = 1500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage = 10.0f;

	// --- 풀링 시스템 ---
	UPROPERTY(EditDefaultsOnly, Category = "Pooling")
	TSubclassOf<ACrossbowBolt> BoltClass; // 화살 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Pooling")
	int32 PoolSize = 10; // 미리 만들어둘 개수

	// --- 내부 로직 변수 ---
	UPROPERTY()
	TArray<ACrossbowBolt*> BoltPool; // 실제 화살들이 담길 배열
	UPROPERTY()
	AActor* CurrentTarget;
	float FireTimer;
	
	// 풀링 초기화
	void InitializePool();
    
	// 풀에서 화살 꺼내오기
	ACrossbowBolt* GetBoltFromPool();

	// 적 감지
	float TargetSearchTimer = 0.0f;
	UFUNCTION()
	void OnEnemyEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEnemyExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 공격
	void Fire();
	void FindBestTarget();

	// 구조체 데이터를 받아서 스탯을 세팅
	void InitializeStructure(const FStructureData& Data);
	// 체력이 변했을 때 호출
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	// 파괴
	void HandleDestruction();

	// 디버그용
	UFUNCTION(CallInEditor, Category = "Debug")
	void Debug_TakeDamage();
};
