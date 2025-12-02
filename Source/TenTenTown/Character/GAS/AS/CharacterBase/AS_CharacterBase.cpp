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
	InitMoveSpeedRate(0.f);
	InitShield(0.f);
	InitDamage(0.f);
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
	else if (Attribute == GetMoveSpeedRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, -999.f, 999.f);
	}
	else if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 999.f);
	}
	else if (Attribute == GetDamageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 999.f);
	}
}

void UAS_CharacterBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		float IncomingDamage = GetDamage();
		SetDamage(0.f);

		if (IncomingDamage <= 0.f)
			return;

		float OldShield = GetShield();
		float NewShield = FMath::Max(OldShield - IncomingDamage, 0.f);
		float Absorbed = OldShield - NewShield;
		
		IncomingDamage -= Absorbed;
		SetShield(NewShield);
        
		if (NewShield <= 0.f && OldShield > 0.f)
		{
			if (ASC && ASC->GetOwnerRole() == ROLE_Authority)
			{
				UE_LOG(LogTemp, Warning, TEXT("Shield Broken"));
			}
		}

		if (IncomingDamage <= 0.f)
			return;
		
		float OldHealth = GetHealth();
		float NewHealth = FMath::Clamp(OldHealth - IncomingDamage, 0.f, GetMaxHealth());

		SetHealth(NewHealth);
        
		if (NewHealth <= 0.f && OldHealth > 0.f)
		{
			if (ASC && ASC->GetOwnerRole() == ROLE_Authority)
			{
				UE_LOG(LogTemp, Warning, TEXT("Dead"));
			}
		}
	}
	
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

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseAtk, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeedRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Shield, COND_None, REPNOTIFY_Always);
}

void UAS_CharacterBase::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Damage, OldDamage);
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

void UAS_CharacterBase::OnRep_MoveSpeedRate(const FGameplayAttributeData& OldMovementRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeedRate, OldMovementRate);
}

void UAS_CharacterBase::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Shield, OldShield);
}	
