#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "CoreStructure.generated.h"

class UAbilitySystemComponent;
class UAS_CoreAttributeSet;
class UBoxComponent;
class UGameplayEffect;

UCLASS()
class TENTENTOWN_API ACoreStructure : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	ACoreStructure();
	
	// IAbilitySystemInterface 구현
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }
protected:
	virtual void BeginPlay() override;

	// 몬스터 감지 시 호출될 함수
	UFUNCTION()
	void OnCoreOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// GAS 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	// 코어 어트리뷰트 셋
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAS_CoreAttributeSet> AttributeSet;

	// 몬스터 감지용 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> DetectCollision;

	// 건물의 외형 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 몬스터에게 피격 시 적용할 GameplayEffect (블루프린트에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
