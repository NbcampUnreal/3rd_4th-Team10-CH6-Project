#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structure/Base/StructureBase.h"
#include "Structure/Crossbow/CrossbowBolt.h"
#include "CrossbowStructure.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class TENTENTOWN_API ACrossbowStructure : public AStructureBase
{
	GENERATED_BODY()
	
public:	
	ACrossbowStructure();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	// 몸체(회전)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TurretMesh;
	// 발사 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzleLocation;
	// 사거리 감지용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DetectSphere;

	// --- 설정값 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackRange = 1500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackSpeed = 1.0f; // 초당 공격 횟수

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
	UFUNCTION()
	void OnEnemyEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEnemyExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 공격
	void Fire();
	void FindBestTarget();

	// [추가] 레벨업 시 스탯 갱신을 위해 오버라이드
	virtual void OnRep_CurrentLevel();
};
