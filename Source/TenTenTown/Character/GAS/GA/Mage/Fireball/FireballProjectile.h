#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireballProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class TENTENTOWN_API AFireballProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AFireballProjectile();
	virtual void BeginPlay() override;

	void InitVelocity(const FVector& Dir, float Speed);
	

protected:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> Move;
	

	//VFX
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> ExplodeVFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	float ExplosionRadius = 250.f;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,const FHitResult& Hit);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Explode(const FVector& ExplodeLoc, const FRotator& ExplodeRot);
	void DoExplode_Server(const FVector& ExplodeLoc, const FRotator& ExplodeRot);
};
