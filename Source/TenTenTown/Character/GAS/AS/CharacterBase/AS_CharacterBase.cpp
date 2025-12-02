#include "AS_CharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Engine/Engine.h"

UAS_CharacterBase::UAS_CharacterBase()
{
	InitBaseAtk(1.f);
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitLevel(1.f);
}

void UAS_CharacterBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UAS_CharacterBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetBaseAtkAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 999.f);
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 99999.f);
	}
	else if (Attribute == GetLevelAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 10.f);
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetEXPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 100.f);
	}
}

void UAS_CharacterBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute==GetEXPAttribute())
	{
		if (GetEXP()>=100.f)
		{
			GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer(GASTAG::Event_LevelUP));
		}
	}
	if (Data.EvaluatedData.Attribute==GetHealthAttribute())
	{
		GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("In PostGE")));
		if (GetHealth()<=KINDA_SMALL_NUMBER)
		{
			GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("In PostGE <=0 HEALTH")));

			FGameplayEventData Payload;
			Payload.EventTag = GASTAG::Event_Character_Dead;
			Payload.Target = GetOwningActor();
			Payload.Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(),GASTAG::Event_Character_Dead,Payload);
		}
	}
}

void UAS_CharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseAtk, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EXP, COND_None, REPNOTIFY_Always);
}

void UAS_CharacterBase::OnRep_BaseAtk(const FGameplayAttributeData& OldBaseAtk)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseAtk, OldBaseAtk);
}

void UAS_CharacterBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldHealth);
}

void UAS_CharacterBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldMaxHealth);
}

void UAS_CharacterBase::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Level, OldLevel);
}

void UAS_CharacterBase::OnRep_EXP(const FGameplayAttributeData& OldEXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EXP, OldEXP);
}	

