#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_CharacterBase.generated.h"

UCLASS()
class TENTENTOWN_API UAS_CharacterBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAS_CharacterBase();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_BaseAtk)
	FGameplayAttributeData BaseAtk;
	ATTRIBUTE_ACCESSORS(ThisClass, BaseAtk);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Attribute", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute", ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(ThisClass, Level);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_EXP)
	FGameplayAttributeData EXP;
	ATTRIBUTE_ACCESSORS(ThisClass, EXP);

	UFUNCTION()
	void OnRep_BaseAtk(const FGameplayAttributeData& OldBaseAtk);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldLevel);

	UFUNCTION()
	void OnRep_EXP(const FGameplayAttributeData& OldEXP);
};
