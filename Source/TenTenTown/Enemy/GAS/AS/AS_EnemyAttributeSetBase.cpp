// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"

#include "GameplayEffectExtension.h"
#include "Engine/Engine.h"
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
	InitExp(7.f);
	
}

void UAS_EnemyAttributeSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UAS_EnemyAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent& ASC = Data.Target;

	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (ASC.HasMatchingGameplayTag(GASTAG::State_Invulnerable))
		{
			return;
		}

		const float IncomingDamage = GetDamage();
		SetDamage(0.0f);
		
		if (IncomingDamage > 0.0f)
		{
			float NewHealth = FMath::Clamp(GetHealth()- IncomingDamage, 0.0f, GetMaxHealth() );

			SetHealth(NewHealth);

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Health : %f"), GetHealth()));

			if (NewHealth <= 0.0f)
			{
			
				if (!ASC.HasMatchingGameplayTag(GASTAG::Enemy_State_Dead))
				{
					if (ASC.GetOwnerRole() == ROLE_Authority)
					{
						ASC.AddLooseGameplayTag(GASTAG::Enemy_State_Dead);
					}

				}
			
				SetHealth(0.0f);
			}
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
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Exp, COND_None, REPNOTIFY_Always);

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

void UAS_EnemyAttributeSetBase::OnRep_Exp(const FGameplayAttributeData& OldExp)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Exp,OldExp);
}
