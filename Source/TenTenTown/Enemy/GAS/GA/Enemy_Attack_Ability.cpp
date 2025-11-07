// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Attack_Ability.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "TTTGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

UEnemy_Attack_Ability::UEnemy_Attack_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	// FGameplayTagContainer Tags = GetAssetTags();
	// Tags.AddTag(GASTAG::Enemy_Ability_Attack);
	// SetAssetTags(Tags);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Attack;
	AbilityTriggers.Add(TriggerData);
}

bool UEnemy_Attack_Ability::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UEnemy_Attack_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// GEngine->AddOnScreenDebugMessage(
	// 	-1,                 
	// 	5.0f,               
	// 	FColor::Yellow,     
	// 	FString::Printf(TEXT("Attack"))
	// );

	if (TriggerEventData)
	{
		AEnemyBase* Actor = const_cast<AEnemyBase*>(Cast<AEnemyBase>(TriggerEventData->Instigator.Get()));
		AActor* TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());

		// 공격시 타겟으로 회전
		FVector TargetLocation = TriggerEventData->Target->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation);
		FRotator NewRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
		
		Actor->SetActorRotation(NewRotation);

		// 공격 애니메이션 재생
		if (!Actor || !Actor->AttackMontage)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		Actor->PlayMontage(Actor->AttackMontage, FMontageEnded(),1.0f);
		Actor->Multicast_PlayMontage(Actor->AttackMontage, 1.0f);

		// 공격 이펙트 적용
		UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
		
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		EffectContext.AddInstigator(Actor, Actor);

		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, -(SourceASC->GetNumericAttributeBase(UAS_EnemyAttributeSetBase::GetAttackAttribute())));

			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

			float TargetHP = TargetASC->GetNumericAttributeBase(UAS_FighterAttributeSet::GetHealthAttribute());
			
			GEngine->AddOnScreenDebugMessage(
				-1,                 
				5.0f,               
				FColor::Yellow,     
				FString::Printf(TEXT("Target HP : %f"), TargetHP)
			);
		}

		
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

}

void UEnemy_Attack_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}