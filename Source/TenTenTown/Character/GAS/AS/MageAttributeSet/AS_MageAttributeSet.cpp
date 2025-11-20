#include "AS_MageAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAS_MageAttributeSet::UAS_MageAttributeSet()
{
	InitBaseAtk(1.f);
	InitHealth(100.f);
	InitMana(100.f);
	InitMaxHealth(100.f);
	InitMaxMana(100.f);
	InitLevel(1.f);
	InitManaRegenRate(1.f);
	InitOverheatingStack(0.f);
}

void UAS_MageAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetBaseAtkAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,999.f);
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,999.f);
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,999.f);
	}
	else if (Attribute == GetManaRegenRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,10.f);
	}
	else if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,10.f);
	}
	else if (Attribute == GetOverheatingStackAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,MaxOverheatingStack);
	}
}

void UAS_MageAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UAS_MageAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void UAS_MageAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseAtk,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ManaRegenRate,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Level,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, OverheatingStack,  COND_None, REPNOTIFY_Always);
}

void UAS_MageAttributeSet::OnRep_BaseAtk(const FGameplayAttributeData& OldBaseAtk)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,BaseAtk,OldBaseAtk);
}

void UAS_MageAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldHealth);
}

void UAS_MageAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Mana,OldMana);
}

void UAS_MageAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxHealth,OldMaxHealth);
}

void UAS_MageAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxMana,OldMaxMana);
}

void UAS_MageAttributeSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,ManaRegenRate,OldManaRegenRate);
}

void UAS_MageAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Level,OldLevel);
}

void UAS_MageAttributeSet::OnRep_OverheatingStack(const FGameplayAttributeData& OldOverheatingStack )
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,OverheatingStack,OldOverheatingStack);
}
