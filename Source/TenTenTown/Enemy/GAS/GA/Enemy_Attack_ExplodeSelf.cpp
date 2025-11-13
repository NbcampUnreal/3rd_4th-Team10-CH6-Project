// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Attack_ExplodeSelf.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/GAS/AS/Beholder_AttributeSet.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UEnemy_Attack_ExplodeSelf::UEnemy_Attack_ExplodeSelf()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Attack;
	AbilityTriggers.Add(TriggerData);
}

bool UEnemy_Attack_ExplodeSelf::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UEnemy_Attack_ExplodeSelf::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		Actor = const_cast<AEnemyBase*>(Cast<AEnemyBase>(TriggerEventData->Instigator.Get()));
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
		UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, Actor->GetAbilitySystemComponent()->GetNumericAttributeBase(UBeholder_AttributeSet::GetAttackSpeedAttribute()));

		WaitTask->OnFinish.AddDynamic(this, &ThisClass::ExecuteExplosion);
		WaitTask->Activate();
		
	}
	
}

void UEnemy_Attack_ExplodeSelf::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemy_Attack_ExplodeSelf::ExecuteExplosion()
{
    if (!Actor)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }
    
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);;
    if (!ASC)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    FGameplayAbilityTargetDataHandle TargetData;
    TArray<TWeakObjectPtr<AActor>> TargetActors;
	
    TArray<FHitResult> HitResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Actor);

    //float ExplodeRange = ASC->GetNumericAttribute(UBeholder_AttributeSet::GetExplodeRangeAttribute());
    float ExplodeRange = 600.f;
    if (ExplodeRange <= 0.0f)
    {
        ExplodeRange = 600.f; 
    }
    
    GetWorld()->SweepMultiByChannel(
        HitResults,
        Actor->GetActorLocation(),
        Actor->GetActorLocation(),
        FQuat::Identity,
        ECollisionChannel::ECC_Pawn, // 이후에 콜리전 채널에 건물포함 필요
        FCollisionShape::MakeSphere(ExplodeRange),
        QueryParams
    );

    float Attack = ASC->GetNumericAttributeBase(UBeholder_AttributeSet::GetAttackAttribute());
    
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddInstigator(Actor, Actor);
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1, EffectContext);

	if (SpecHandle.IsValid()) 
	{
		SpecHandle.Data->SetSetByCallerMagnitude(GASTAG::Data_Enemy_Damage, -Attack);
		
		for (const FHitResult& Hit : HitResults)
		{
			if (ACharacter* TargetCharacter = Cast<ACharacter>(Hit.GetActor()))
			{
				UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetCharacter);
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
    
		// 비주얼 이펙트 추가
	
		//Actor->Destroy();
		ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Dead);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
