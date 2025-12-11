// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ArcherNormalAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Characters/Archer/ArcherCharacter/ArcherCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UGA_ArcherNormalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	ArcherCharacter = Cast<AArcherCharacter>(GetAvatarActorFromActorInfo());
	Bow = ArcherCharacter->GetEquippedBow();
	AnimInstance = ArcherCharacter->GetMesh()->GetAnimInstance();
	
	if (!ASC || !ArcherCharacter || !Bow)
	{
		UE_LOG(LogTemp, Warning, TEXT("Validation Failed: ASC[%s], Character[%s], Bow[%s]"), 
			ASC ? TEXT("Valid") : TEXT("NULL"), 
			ArcherCharacter ? TEXT("Valid") : TEXT("NULL"), 
			Bow ? TEXT("Valid") : TEXT("NULL")
			);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* AMTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("None"),AttackMontage,1.f,NAME_None,true,1.f,0.f);
	AMTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	AMTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	AMTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	
	AMTask->ReadyForActivation();
	ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Archer_NormalAttackStart);
	ASC->ForceReplication();
}
void UGA_ArcherNormalAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ArcherNormalAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UGA_ArcherNormalAttack::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (ASC)
	{
		ASC->CurrentMontageJumpToSection("Release");
		ASC->ExecuteGameplayCue(GASTAG::GameplayCue_Archer_NormalAttackRelease);
	}
	ASC->ForceReplication();
}

void UGA_ArcherNormalAttack::OnMontageEnd()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
