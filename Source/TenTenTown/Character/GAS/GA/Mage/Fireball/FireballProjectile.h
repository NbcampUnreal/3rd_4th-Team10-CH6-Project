#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireballProjectile.generated.h"

class UGameplayEffect;
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
	
	UPROPERTY()
	bool bCountsForOverheat = true;
	
	void InitVelocity(const FVector& Dir, float Speed);

	
protected:
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> Move;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DamageGE;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag Tag_Damage;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageAmount = 40.f;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamageMultiplier = 1.f;

	//VFX
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> ExplodeVFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	float ProjectileTraceRadius = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	float ExplosionRadius = 400.f;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,const FHitResult& Hit);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Explode(const FVector& ExplodeLoc, const FRotator& ExplodeRot);
	void DoExplode_Server(const FVector& ExplodeLoc, const FRotator& ExplodeRot);
};