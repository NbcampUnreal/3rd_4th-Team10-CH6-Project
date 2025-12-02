// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Counter_Ability.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/EnemyList/BlackKnight.h"
#include "Enemy/GAS/AS/BlackKnight_AttributeSet.h"
#include "Kismet/KismetMathLibrary.h"

UEnemy_Counter_Ability::UEnemy_Counter_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GASTAG::Enemy_Ability_Counter;
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.AddTag(GASTAG::Enemy_State_Dead);
}

void UEnemy_Counter_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (TriggerEventData)
	{
		Actor = const_cast<ABlackKnight*>(Cast<ABlackKnight>(TriggerEventData->Instigator.Get()));
		CurrentTarget = const_cast<AActor*>(TriggerEventData->Target.Get());

		if (CurrentTarget)
		{
			FVector TargetLocation = CurrentTarget->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation);
			Actor->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
		}

		PlayAttackMontage();
	}
}

void UEnemy_Counter_Ability::PlayAttackMontage()
{
	ABlackKnight* BlackKnight = Cast<ABlackKnight>(Actor);
	if (!BlackKnight || !BlackKnight->CounterMontage) 
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		AttackSpeed = ASC->GetNumericAttribute(UAS_EnemyAttributeSetBase::GetAttackSpeedAttribute());
	}

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		BlackKnight->CounterMontage, 
		AttackSpeed
	);

	if (!Task)
	{
		return;
	}
	
	Task->OnCompleted.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);
	Task->OnBlendOut.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);
	Task->OnCancelled.AddDynamic(this, &UEnemy_Counter_Ability::OnMontageEnded);

	UAnimInstance* AnimInst = BlackKnight->GetMesh()->GetAnimInstance();
	if (AnimInst && !AnimInst->OnPlayMontageNotifyBegin.IsAlreadyBound(this, &UEnemy_Counter_Ability::OnNotifyBegin))
	{
		AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UEnemy_Counter_Ability::OnNotifyBegin);
	}

	Task->ReadyForActivation();
}
void UEnemy_Counter_Ability::OnMontageEnded()
{
	if (!Actor)
	{
		return;
	}
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Counter);
		ASC->SetNumericAttributeBase(UBlackKnight_AttributeSet::GetFrontHitCountAttribute(),0.f);
	}
	
	if (Actor && Actor->GetMesh() && Actor->GetMesh()->GetAnimInstance())
	{
		Actor->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UEnemy_Counter_Ability::OnNotifyBegin);
	}


	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
