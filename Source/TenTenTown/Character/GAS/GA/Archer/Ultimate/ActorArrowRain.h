// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorArrowRain.generated.h"

class AArcher_Arrow;

UCLASS()
class TENTENTOWN_API AActorArrowRain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorArrowRain();
	void SetArrowClass(TSubclassOf<AArcher_Arrow> NewArrowClass);
	void InitializeArrowRain(float InDamage, TSubclassOf<class UGameplayEffect> InGEClass);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// --- 설정값 ---
	UPROPERTY(EditAnywhere, Category = "ArrowRain")
	float RainRadius = 900.f; 

	UPROPERTY(EditAnywhere, Category = "ArrowRain")
	float SpawnHeight = 3500.f; 

	UPROPERTY(EditAnywhere, Category = "ArrowRain")
	int32 TotalArrowCount = 50; // 총 화살 개수

	UPROPERTY(EditAnywhere, Category = "ArrowRain")
	float SpawnInterval = 0.1f; // 화살 사이의 시간 간격
	
	UPROPERTY(EditAnywhere, Category = "ArrowRain")
	float ArrowDamage = 50.f;
	
	UPROPERTY(EditAnywhere, Category = "ArrowRain", meta = (ClampMin = "0.0", ClampMax = "45.0"))
	float MaxTiltAngle = 15.0f;

	UPROPERTY(EditAnywhere, Category = "ArrowRain")
	TSubclassOf<class UGameplayEffect> DamageGEClass;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ArrowClass")
	TSubclassOf<AArcher_Arrow> ArrowClass;
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTimerHandle RainTimerHandle;
	int32 ArrowsSpawnedSoFar = 0;

	void SpawnSingleArrow(); // 실제 화살을 스폰하는 내부 함수
};
