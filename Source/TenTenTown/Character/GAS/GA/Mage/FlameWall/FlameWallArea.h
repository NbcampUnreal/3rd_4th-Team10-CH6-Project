#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlameWallArea.generated.h"

class UGameplayEffect;

UCLASS()
class TENTENTOWN_API AFlameWallArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlameWallArea();

	void Init(float InLifeTime);

	UFUNCTION()
	void OnDamageZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDamageZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DotGE;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag Tag_DoT;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTagContainer DotGrantedTags;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamagePerTick = 5.f;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditDefaultsOnly, Category="Wall")
	float Lifetime = 5.f;
};
