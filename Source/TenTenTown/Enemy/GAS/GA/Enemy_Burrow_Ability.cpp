// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/GA/Enemy_Burrow_Ability.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/Base/EnemyBase.h"
#include "Enemy/EnemyList/Worm.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

UEnemy_Burrow_Ability::UEnemy_Burrow_Ability()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

    AbilityTags.AddTag(GASTAG::Enemy_Ability_Burrow);
}

void UEnemy_Burrow_Ability::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AActor* Actor = GetAvatarActorFromActorInfo();
    if (!Actor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    if (!Actor->HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("UEnemy_Burrow_Ability::ActivateAbility called but Avatar has no authority"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AWorm* Worm = Cast<AWorm>(Actor);
    if (!Worm)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UAnimMontage* BurrowMontage = Worm->BurrowMontage;
    if (!BurrowMontage)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("BurrowAbility Activated (Server)"));

    // NOTE: rely on GE tags (not AddLooseGameplayTag) - we will apply effects later which add tags
    // Register notify delegate safely (prevent duplicates)
    if (USkeletalMeshComponent* Mesh = Worm->GetMesh())
    {
        if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
        {
            AnimInst->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ThisClass::OnNotifyBegin);
            AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::OnNotifyBegin);
        }
    }

    // Play montage via AbilityTask
    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, BurrowMontage, 0.8f);

    MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnBurrowMontageFinished);
    MontageTask->OnCancelled.AddDynamic(this, &ThisClass::CleanupState);
    MontageTask->Activate();
}

void UEnemy_Burrow_Ability::OnBurrowMontageFinished()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    AActor* Actor = GetAvatarActorFromActorInfo();
    if (!ASC || !Actor) return;

    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddInstigator(Actor, Actor);

    if (InvulnerableEffect)
    {
        FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(InvulnerableEffect, GetAbilityLevel(), EffectContext);
        if (Spec.IsValid())
        {
            ActiveInvulnerableGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        }
    }

    if (BurrowEffect)
    {
        FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(BurrowEffect, GetAbilityLevel(), EffectContext);
        if (Spec.IsValid())
        {
            ActiveBurrowGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Burrow GEs Applied (Server)."));
    // Keep ability active until unburrow (EndAbility is called on unburrow)
}

void UEnemy_Burrow_Ability::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
    // Play unburrow montage if available, else cleanup immediately
    AActor* Actor = GetAvatarActorFromActorInfo();
    AWorm* Worm = Cast<AWorm>(Actor);
    if (Worm && Worm->UnBurrowMontage)
    {
        UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            Worm->UnBurrowMontage,
            1.0f);
        
        MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnUnBurrowMontageFinished);
        MontageTask->Activate();
    }
    else
    {
        CleanupState();
        Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    }
}

void UEnemy_Burrow_Ability::OnUnBurrowMontageFinished()
{
    CleanupState();
    Super::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UEnemy_Burrow_Ability::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateCancelAbility)
{
    OnUnBurrowMontageFinished();
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UEnemy_Burrow_Ability::CleanupState()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    AActor* Actor = GetAvatarActorFromActorInfo();

    if (AWorm* Worm = Cast<AWorm>(Actor))
    {
        if (USkeletalMeshComponent* Mesh = Worm->GetMesh())
        {
            if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
            {
                AnimInst->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ThisClass::OnNotifyBegin);
            }
        }
    }

    if (ASC)
    {
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
    }

    if (AWorm* Worm = Cast<AWorm>(GetAvatarActorFromActorInfo()))
    {
        if (USkeletalMeshComponent* Mesh = Worm->GetMesh())
        {
            Mesh->SetVisibility(true);
        }
        if (UCapsuleComponent* Capsule = Worm->GetCapsuleComponent())
        {
            Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
    }
}

void UEnemy_Burrow_Ability::OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    AEnemyBase* Actor = Cast<AEnemyBase>(GetAvatarActorFromActorInfo());

    if (!Actor) return;

    if (NotifyName == FName("BurrowStart"))
    {
        if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
        {
            Mesh->SetVisibility(false);
        }
        if (UCapsuleComponent* Capsule = Actor->GetCapsuleComponent())
        {
            Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
    else if (NotifyName == FName("BurrowEnd"))
    {
        if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
        {
            Mesh->SetVisibility(true);
        }
        if (UCapsuleComponent* Capsule = Actor->GetCapsuleComponent())
        {
            Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
    }
}
