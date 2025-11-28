#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AtonementActor.generated.h"

class UAbilitySystemComponent;
class ABaseCharacter;
class UGameplayEffect;
class UNiagaraComponent;
class USphereComponent;

UCLASS()
class TENTENTOWN_API AAtonementActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAtonementActor();

	UFUNCTION()
	void OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	virtual void BeginPlay() override;
	
	//Character GE
	UPROPERTY(EditDefaultsOnly, Category="Atonement|GAS")
	TSubclassOf<UGameplayEffect> ShieldGE;
	UPROPERTY(EditDefaultsOnly, Category="Atonement|GAS")
	TSubclassOf<UGameplayEffect> ShieldActiveGE;
	UPROPERTY(EditDefaultsOnly, Category="Atonement|GAS")
	TSubclassOf<UGameplayEffect> SpeedUpGE;

	UPROPERTY()
	TSet<TWeakObjectPtr<ABaseCharacter>> AlreadyShieldedChars;
	
	//Enemy GE
	UPROPERTY(EditDefaultsOnly, Category="Atonement|GAS")
	TSubclassOf<UGameplayEffect> SlowGE;
	UPROPERTY(EditDefaultsOnly, Category="Atonement|GAS")
	TSubclassOf<UGameplayEffect> VulnGE;
	
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UNiagaraComponent> AreaVFX;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* Area = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	float AoERadius = 600.f;
	UPROPERTY(EditDefaultsOnly)
	float AoEHalfHeight = 30.f;
	UPROPERTY(EditDefaultsOnly)
	float AoELifeTime = 15.f;

	FGameplayTag ShieldTag = FGameplayTag::RequestGameplayTag(FName("Data.Buff.Shield"));
	FGameplayTag ShieldActiveTag = FGameplayTag::RequestGameplayTag(FName("State.Buff.Shield"));
	FGameplayTag SpeedUpTag = FGameplayTag::RequestGameplayTag(FName("Data.Buff.SpeedUp"));
	FGameplayTag SlowTag = FGameplayTag::RequestGameplayTag(FName("Data.Debuff.Slow"));
	FGameplayTag VulnTag = FGameplayTag::RequestGameplayTag(FName("Data.Debuff.Vuln"));
	
	UPROPERTY(EditAnywhere, Category="Atonement|Buff")
	float ShieldAmount = 50.f;
	UPROPERTY(EditAnywhere, Category="Atonement|Buff")
	float SpeedUpRate = 0.5f;
	UPROPERTY(EditAnywhere, Category="Atonement|Debuff")
	float SlowRate = 0.5f;
	UPROPERTY(EditAnywhere, Category="Atonement|Debuff")
	float VulnerabilityRate = 0.25f;

	TSet<TWeakObjectPtr<ABaseCharacter>> CharsInArea;

	UPROPERTY(EditAnywhere, Category="Atonement|Buff")
	float SpeedUpRefreshInterval = 0.5f;

	FTimerHandle SpeedUpRefreshTimerHandle;
	
	void ApplyGEToASC(UAbilitySystemComponent* TargetASC, TSubclassOf<UGameplayEffect> GEClass, float Level, FGameplayTag SetByCallerTag, float SetByCallerValue) const;
	void RefreshSpeedUpBuffs();
};
