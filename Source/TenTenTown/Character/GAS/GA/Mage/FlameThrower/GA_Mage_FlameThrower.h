#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GAS/GA/Mage/Base/GA_Mage_Base.h"
#include "GA_Mage_FlameThrower.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class AFlameThrowerActor;

UCLASS()
class TENTENTOWN_API UGA_Mage_FlameThrower : public UGA_Mage_Base
{
	GENERATED_BODY()

public:
	UGA_Mage_FlameThrower();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Flame Thrower")
	TSubclassOf<AFlameThrowerActor> FlameClass;
	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	float ChargeHoldTime = 1.0f;
	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	float MaxChannelTime = 5.0f;
	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	float Range = 1200.f;
	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	float ConeHalfAngleDeg = 30.f;
	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	float DPS = 180.0f;
	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	float TickInterval = 0.05f;
	UPROPERTY(EditDefaultsOnly,Category="Flame Thrower")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditDefaultsOnly, Category="Flame Thrower")
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(EditDefaultsOnly, Category="Flame|Set")
	float GroundTraceUp = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category="Flame|Set")
	float GroundTraceDown = 6000.f;
	UPROPERTY(EditDefaultsOnly, Category="Flame|Set")
	float GroundOffset = 5.f;

	UPROPERTY()
	TObjectPtr<AFlameThrowerActor> SpawnedActor = nullptr;
	
	FTimerHandle ChargeTimer;
	FTimerHandle ChannelTimer;

	bool bInputHeld = false;
	
	void OnShootEvent();
	
	UFUNCTION(Server, Reliable)
	void ServerSpawnFlame();
	void SpawnFlame();
	
	UFUNCTION()
	void OnChargeComplete();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	EMovementMode SavedMoveMode = MOVE_Walking;
};
