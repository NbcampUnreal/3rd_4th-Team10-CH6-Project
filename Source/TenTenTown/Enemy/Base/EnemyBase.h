// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

class ASplineActor;
class USoundCue;
class AEnemyProjectileBase;
class ATestGold;
class UAnimInstance;
class UAnimMontage;
class USphereComponent;
class UAbilitySystemComponent;
class UStateTreeComponent;
class UCapsuleComponent;
class UAttributeSet;
class UGameplayAbility;

DECLARE_DYNAMIC_DELEGATE_OneParam(FMontageEnded, UAnimMontage*, Montage);

UCLASS()
class TENTENTOWN_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FMontageEnded OnMontageEndedDelegate;
	
public:
	AEnemyBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	float MovedDistance = 0.f;

	UPROPERTY(Replicated)
	float DistanceOffset = 0.f;

	UPROPERTY()
	int32 SpawnWaveIndex = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Drop")
	TSubclassOf<ATestGold> GoldItem;

	virtual void InitializeEnemy();

	virtual void ResetEnemy();
protected:
	virtual void BeginPlay() override;
	//virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	
	
	//Event
	UFUNCTION()
	void OnDetection(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndDetection(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void SetCombatTagStatus(bool IsCombat);

	TArray<TWeakObjectPtr<AActor>> OverlappedPawns;

private:
	void AddDefaultAbility();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GAS")
	const UAS_EnemyAttributeSetBase* DefaultAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Attributes")
	TObjectPtr<UDataTable> DataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Attributes")
	FName DataTableRowName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UStateTreeComponent> StateTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detection")
	TObjectPtr<USphereComponent> DetectComponent;

public:
	// Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeadMontage;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayMontage(UAnimMontage* MontageToPlay, float InPlayRate);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	float PlayMontage(UAnimMontage* MontageToPlay, FMontageEnded Delegate, float InPlayRate = 1.f);

	// Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundCue> AttackSound;
	
	// ItemDrop
	
	void DropGoldItem();

	//distance와 offset복제
	UPROPERTY(Replicated)
	TObjectPtr<ASplineActor> SplineActor;
	
	UFUNCTION()
	void OnRep_MovedDistance();

	UFUNCTION()
	void OnRep_DistanceOffset();

	void ApplySplineMovementCorrection();
	
	// Range Only
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	TSubclassOf<AEnemyProjectileBase> RangedProjectileClass;

	TSubclassOf<AEnemyProjectileBase> GetRangedProjectileClass() const { return RangedProjectileClass; }
public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	const TArray<TWeakObjectPtr<AActor>>& GetOverlappedPawns() const { return OverlappedPawns; }

	UFUNCTION(BlueprintCallable)
	const UAS_EnemyAttributeSetBase* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable)
	void StartTree();

private:
	/** 3초 로그 출력을 위한 타이머 */
	float LogTimer = 0.0f;

	void LogAttributeAndTags();
};
