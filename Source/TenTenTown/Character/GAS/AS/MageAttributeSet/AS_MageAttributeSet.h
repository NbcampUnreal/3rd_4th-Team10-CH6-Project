#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AS_MageAttributeSet.generated.h"

UCLASS()
class TENTENTOWN_API UAS_MageAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UAS_MageAttributeSet();
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing= OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(ThisClass,Level);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ThisClass,Health);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(ThisClass,Mana);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ThisClass,MaxHealth);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(ThisClass,MaxMana);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(ThisClass,ManaRegenRate);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Attribute",ReplicatedUsing = OnRep_OverheatingStack)
	FGameplayAttributeData OverheatingStack;
	ATTRIBUTE_ACCESSORS(ThisClass,OverheatingStack);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="OverheatingStack")
	float NeedOverheatingStack = 10.f;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="OverheatingStack")
	float MaxOverheatingStack = 50.f;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
	
	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldLevel);

	UFUNCTION()
	void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate);
	
	UFUNCTION()
	void OnRep_OverheatingStack(const FGameplayAttributeData& OldOverheatingStack);
};
