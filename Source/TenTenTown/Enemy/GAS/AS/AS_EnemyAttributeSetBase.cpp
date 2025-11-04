// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Net/UnrealNetwork.h"


UAS_EnemyAttributeSetBase::UAS_EnemyAttributeSetBase()
{
	InitHealth(777.f);
	InitMaxHealth(777.f);
	InitAttack(777.f);
	InitAttackSpeed(7.f);
	InitMovementSpeed(777.f);
	InitAttackRange(777.f);
	InitGold(777.f);
	
}

void UAS_EnemyAttributeSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UAS_EnemyAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UAS_EnemyAttributeSetBase::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue,
	float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
}

void UAS_EnemyAttributeSetBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Gold, COND_None, REPNOTIFY_Always);

}



void UAS_EnemyAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldHealth);

}

void UAS_EnemyAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldMaxHealth);
}

void UAS_EnemyAttributeSetBase::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldAttack);
}

void UAS_EnemyAttributeSetBase::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldMovementSpeed);
}

void UAS_EnemyAttributeSetBase::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldAttackSpeed);
}

void UAS_EnemyAttributeSetBase::OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,AttackRange,OldAttackRange);
}

void UAS_EnemyAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldGold);
}
