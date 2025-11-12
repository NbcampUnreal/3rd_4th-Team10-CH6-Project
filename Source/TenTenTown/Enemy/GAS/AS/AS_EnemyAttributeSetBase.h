// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_EnemyAttributeSetBase.generated.h"

/**
 * 
 */
UCLASS()
class TENTENTOWN_API UAS_EnemyAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

	UAS_EnemyAttributeSetBase();

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute", ReplicatedUsing= OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass, Health);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(ThisClass, Attack);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(ThisClass, MovementSpeed);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(ThisClass, AttackSpeed);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_AttackRange)
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(ThisClass, AttackRange);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(ThisClass, Gold);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldAttack);
	
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
	
	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed);

	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange);
	
	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldGold);
	
};
