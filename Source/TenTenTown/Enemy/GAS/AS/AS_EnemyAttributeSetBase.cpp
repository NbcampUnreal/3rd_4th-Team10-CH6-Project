// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"

#include "GameplayEffectExtension.h"
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float CurrentHealth = GetHealth();

		UAbilitySystemComponent& TargetASC = Data.Target;
		
		if (CurrentHealth <= 0.0f)
		{
			if (!TargetASC.HasMatchingGameplayTag(GASTAG::Enemy_State_Dead))
			{
				if (TargetASC.GetOwnerRole() == ROLE_Authority)
				{
					TargetASC.AddLooseGameplayTag(GASTAG::Enemy_State_Dead);
				}

			}
			SetHealth(0.0f);
		}
	}
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
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxHealth,OldMaxHealth);
}

void UAS_EnemyAttributeSetBase::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Attack,OldAttack);
}

void UAS_EnemyAttributeSetBase::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MovementSpeed,OldMovementSpeed);
}

void UAS_EnemyAttributeSetBase::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,AttackSpeed,OldAttackSpeed);
}

void UAS_EnemyAttributeSetBase::OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,AttackRange,OldAttackRange);
}

void UAS_EnemyAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Gold,OldGold);
}
