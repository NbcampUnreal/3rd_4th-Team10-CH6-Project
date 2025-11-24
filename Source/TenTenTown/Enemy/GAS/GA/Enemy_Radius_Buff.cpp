// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Radius_Buff.h"

#include "AbilitySystemComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/NagaWizard_AttributeSet.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

UEnemy_Radius_Buff::UEnemy_Radius_Buff()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(GASTAG::Enemy_Ability_BuffNearBy);
	SetAssetTags(Tags);
}

bool UEnemy_Radius_Buff::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                            const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}


void UEnemy_Radius_Buff::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AActor* Actor = GetAvatarActorFromActorInfo();

	if (!ASC || !Actor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TArray<AActor*> OverlapActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; 
	TArray<AActor*> IgnoreActors;
	

	bool bResult = UKismetSystemLibrary::SphereOverlapActors(
		Actor->GetWorld(),
		Actor->GetActorLocation(),
		ASC->GetNumericAttribute(UNagaWizard_AttributeSet::GetBuffRadiusAttribute()),
		ObjectTypes, 
		AEnemyBase::StaticClass(),
		IgnoreActors,
		OverlapActors
	);

	if (bResult)
	{
		for (AActor* TargetActor : OverlapActors)
		{
			AEnemyBase* TargetEnemy = Cast<AEnemyBase>(TargetActor);

			if (TargetEnemy)
			{
				UAbilitySystemComponent* TargetASC = TargetEnemy->GetAbilitySystemComponent();
             
				if (TargetASC)
				{
					FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
					EffectContext.AddInstigator(Actor, Actor);

					
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
					   BuffEffect, 
					   GetAbilityLevel(), 
					   EffectContext
					);

					if (SpecHandle.IsValid())
					{
						SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Enemy_Ability_BuffNearBy, (ASC->GetNumericAttributeBase(UNagaWizard_AttributeSet::GetBuffScaleAttribute())));

						ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
						
						// TargetASC->ExecuteGameplayCue(GASTAG::GameplayCue_Enemy_Buff, Params);
					}
				}
			}
		}
	}
	

	//if (CooldownEffect)
	//{
	//	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	//	EffectContext.AddInstigator(AvatarActor, AvatarActor);
//
	//	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
	//	   CooldownEffect, 
	//	   GetAbilityLevel(), 
	//	   EffectContext
	//	);
//
	//	if (SpecHandle.IsValid())
	//	{
	//		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	//	}
	//}
}

void UEnemy_Radius_Buff::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
