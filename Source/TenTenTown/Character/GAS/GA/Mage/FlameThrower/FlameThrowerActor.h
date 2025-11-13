#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlameThrowerActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class ACharacter;

UCLASS()
class TENTENTOWN_API AFlameThrowerActor : public AActor
{
	GENERATED_BODY()

public:
	AFlameThrowerActor();

	void Init(float InInterval, float InDPS, float InConeHalfAngleDeg, float InMaxChannelTime);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* Root;
	UPROPERTY(VisibleDefaultsOnly)
	UNiagaraComponent* VFX;
	UPROPERTY(EditDefaultsOnly)
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(ReplicatedUsing=OnRep_Firing)
	bool bFiring = false;
	
	UPROPERTY(Replicated)
	float Range = 1200.f;
	UPROPERTY(Replicated)
	float ConeHalfAngleDeg = 30.f;

	TWeakObjectPtr<ACharacter> OwnerChar;
	float TickInterval = 0.05f;
	float DamagePerTick = 9.f;
	ECollisionChannel TraceChannel = ECC_Visibility;

	FTimerHandle TickTimer;

	UFUNCTION()
	void OnRep_Firing();
	void EndPlay(EEndPlayReason::Type Reason);
	
	void ServerTickDamage();

	bool GetStartAndDir(FVector& OutStart, FVector& OutDir) const;

};
