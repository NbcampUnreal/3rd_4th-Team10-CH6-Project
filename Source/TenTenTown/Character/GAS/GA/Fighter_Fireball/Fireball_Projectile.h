#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "GameFramework/Actor.h"
#include "Fireball_Projectile.generated.h"

class UAbilitySystemComponent;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
UCLASS()
class TENTENTOWN_API AFireball_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AFireball_Projectile();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	//컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	//속도 및 중력 스케일
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float InitialSpeed;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float MaxSpeed;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	bool AlignActorWithVelocity;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float GravityScale;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float CollisionRadius;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Var")
	float LifeSpan;
	
	UFUNCTION(BlueprintCallable)
	void FireProjectile(const FVector& Start, const FVector& End,AActor* IgnoreActor);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnStop(const FHitResult& HitResult);
	
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	UPROPERTY()
	FTimerHandle OnTimeOut;
};
