#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireballProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class TENTENTOWN_API AFireballProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AFireballProjectile();

	void InitVelocity(const FVector& Dir, float Speed);

protected:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> Move;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,const FHitResult& Hit);
};
