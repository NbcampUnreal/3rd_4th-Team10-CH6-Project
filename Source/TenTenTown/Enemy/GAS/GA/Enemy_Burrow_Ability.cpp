// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Burrow_Ability.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/EnemyList/Worm.h"

UEnemy_Burrow_Ability::UEnemy_Burrow_Ability()
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(GASTAG::Enemy_Ability_Burrow);
	SetAssetTags(Tags);
}

void UEnemy_Burrow_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AWorm* Actor = Cast<AWorm>(GetAvatarActorFromActorInfo());
	UAnimMontage* BurrowMontage = Actor->BurrowMontage;
	
	if (!Actor || !BurrowMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(GASTAG::Enemy_State_Burrowed);
    
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, BurrowMontage, 0.8f);

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnBurrowMontageFinished);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::CleanupState);

	MontageTask->Activate();
}

void UEnemy_Burrow_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	AWorm* Actor = Cast<AWorm>(GetAvatarActorFromActorInfo());
	UAnimMontage* UnBurrowMontage = Actor->UnBurrowMontage; 
	
	if (UnBurrowMontage)
	{
		if (Actor)
		{
			UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, NAME_None, UnBurrowMontage, 1.0f);
            
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnUnBurrowMontageFinished);
			MontageTask->Activate();
			
		}
	}
	
	CleanupState();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemy_Burrow_Ability::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	AWorm* Actor = Cast<AWorm>(GetAvatarActorFromActorInfo());
	UAnimMontage* UnBurrowMontage = Actor ? Actor->UnBurrowMontage : nullptr;
    
	if (UnBurrowMontage && Actor)
	{
		if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
		{
			Mesh->SetVisibility(true);
		}
         
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("UnBurrowing"), UnBurrowMontage, 1.0f);
            
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnUnBurrowMontageFinished);
		MontageTask->Activate();

		//bIsCanceling = true; // 플래그 설정
         
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	}
	else
	{
		CleanupState();
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	}
}

void UEnemy_Burrow_Ability::OnBurrowMontageFinished()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AEnemyBase* Actor = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddInstigator(Actor, Actor);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InvulnerableEffect, GetAbilityLevel(), EffectContext);
	if (SpecHandle.IsValid())
	{
		ActiveInvulnerableGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	SpecHandle = ASC->MakeOutgoingSpec(BurrowEffect, GetAbilityLevel(), EffectContext);
	if (SpecHandle.IsValid())
	{
		ActiveBurrowGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	if (ASC && Actor)
	{
		ASC->AddLooseGameplayTag(GASTAG::State_Invulnerable);
		ASC->AddLooseGameplayTag(GASTAG::GameplayCue_Enemy_Effect_Burrow);
        
		if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
		{
			Mesh->SetVisibility(false);
		}
		if (UCapsuleComponent* Capsule = Actor->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void UEnemy_Burrow_Ability::OnUnBurrowMontageFinished()
{
	CleanupState();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}



void UEnemy_Burrow_Ability::CleanupState()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AEnemyBase* Enemy = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());

	if (ActiveInvulnerableGEHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActiveInvulnerableGEHandle);
		ActiveInvulnerableGEHandle.Invalidate();
	}

	if (ActiveBurrowGEHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActiveBurrowGEHandle);
		ActiveBurrowGEHandle.Invalidate();
	}
	
	if (ASC && ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Burrowed))
	{
		ASC->RemoveLooseGameplayTag(GASTAG::Enemy_State_Burrowed);
		ASC->RemoveLooseGameplayTag(GASTAG::State_Invulnerable);
		ASC->RemoveLooseGameplayTag(GASTAG::GameplayCue_Enemy_Effect_Burrow);
	}
    
	if (Enemy)
	{
		if (USkeletalMeshComponent* Mesh = Enemy->GetMesh())
		{
			Mesh->SetVisibility(true);
		}
		if (UCapsuleComponent* Capsule = Enemy->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}
