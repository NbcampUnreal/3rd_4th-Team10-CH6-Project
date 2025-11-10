#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/GA/Mage/Base/GA_Mage_Base.h"
#include "GA_Mage_Fireball.generated.h"

class AFireballProjectile;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class TENTENTOWN_API UGA_Mage_Fireball : public UGA_Mage_Base
{
	GENERATED_BODY()

public:
	UGA_Mage_Fireball();

	UPROPERTY(EditDefaultsOnly, Category = "Fireball")
	TSubclassOf<AFireballProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Anim")
	TObjectPtr<UAnimMontage> FireballMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	FGameplayTag ShootTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Mage.Fireball.Shoot"));

	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	FName MuzzleSocketName = TEXT("Muzzle");
	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	bool bUseCameraAim = false;
	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	float SpawnForwardOffset = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float MuzzleSpeed = 2400.f;
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnShootEvent(const FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(const FVector& SpawnLoc, const FRotator& SpawnRot);
	void SpawnFireball(const FVector& SpawnLoc, const FRotator& ViewRot);
	

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayTask;
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitTask;
};
