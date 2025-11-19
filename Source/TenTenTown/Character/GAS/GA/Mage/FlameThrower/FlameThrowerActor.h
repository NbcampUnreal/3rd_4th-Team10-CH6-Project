#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlameThrowerActor.generated.h"

class UGameplayEffect;
class UNiagaraComponent;
class UNiagaraSystem;
class ACharacter;
class UBoxComponent;

UCLASS()
class TENTENTOWN_API AFlameThrowerActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnDamageZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDamageZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	AFlameThrowerActor();
	
	void Init(float InInterval, float InConeHalfAngleDeg, float InMaxChannelTime);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<UGameplayEffect> DotGE;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag Tag_DoT;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTagContainer DotGrantedTags;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DamagePerTick = 9.f;
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> BurningEnemies;
	
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* Root;
	UPROPERTY(VisibleDefaultsOnly)
	UNiagaraComponent* VFX;
	UPROPERTY(VisibleDefaultsOnly, Category="Flame|Collision")
	UBoxComponent* DamageZone = nullptr;
	UPROPERTY(EditDefaultsOnly)
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(ReplicatedUsing=OnRep_Firing)
	bool bFiring = false;
	
	UPROPERTY(Replicated)
	float Range = 1200.f;
	UPROPERTY(Replicated)
	float ConeHalfAngleDeg = 5.f;

	TWeakObjectPtr<ACharacter> OwnerChar;
	float TickInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="Overheat")
	float OverheatStackInterval = 0.5f;
	float OverheatTimeAcc = 0.f;

	ECollisionChannel TraceChannel = ECC_Visibility;

	FTimerHandle TickTimer;

	UFUNCTION()
	void OnRep_Firing();
	void EndPlay(EEndPlayReason::Type Reason);
	
	void ServerTickDamage();

	bool GetStartAndDir(FVector& OutStart, FVector& OutDir) const;

	void UpdateDamageZoneTransform();
};
