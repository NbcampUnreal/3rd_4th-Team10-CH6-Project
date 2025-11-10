// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "GA_FighterNormalAttack.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"

void UGA_FighterNormalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	
	AnimInstance = Cast<ACharacter>(GetAvatarActorFromActorInfo())->GetMesh()->GetAnimInstance();
	if (!AnimInstance) EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	
	SectionNames = {"First","Second"};
	bIsComboSectionStart = false;
	bIsComboInputPressed = false;
	CurrentComboCount = 0;
	
	auto* ComboStartWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fighter_ComboStart);
	auto* ComboEndWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fighter_ComboEnd);
	auto* FirstSecondComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("FirstSecondComboMontageTask"),FirstSecondComboMontage,1.f,NAME_None,true
		,1.f,0.f,true);

	ComboStartWaitTask->EventReceived.AddUniqueDynamic(this,&ThisClass::ComboStart);
	ComboEndWaitTask->EventReceived.AddUniqueDynamic(this,&ThisClass::ComboEnd);
	FirstSecondComboMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnFirstSecondMontageEnd);
	FirstSecondComboMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnInterrupted);
	ComboStartWaitTask->ReadyForActivation();
	ComboEndWaitTask->ReadyForActivation();
	FirstSecondComboMontageTask->ReadyForActivation();
	
	GEngine->AddOnScreenDebugMessage(55,10.f,FColor::Green,FString::Printf(TEXT("%d"),CurrentComboCount));

	ASC->ForceReplication();
}

void UGA_FighterNormalAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FighterNormalAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	if (!bIsComboSectionStart) return;

	if (!bIsComboInputPressed)
	{
		if (CurrentComboCount==0)
		{
			ASC->CurrentMontageSetNextSectionName(SectionNames[CurrentComboCount],SectionNames[CurrentComboCount+1]);
		}
		CurrentComboCount++;
		bIsComboInputPressed = true;
	}

	GEngine->AddOnScreenDebugMessage(55,10.f,FColor::Green,FString::Printf(TEXT("%d"),CurrentComboCount));
	ASC->ForceReplication();
}

void UGA_FighterNormalAttack::ComboStart(const FGameplayEventData Data)
{
	bIsComboSectionStart=true;
}

void UGA_FighterNormalAttack::ComboEnd(const FGameplayEventData Data)
{
	bIsComboSectionStart=false;
	bIsComboInputPressed=false;
}

void UGA_FighterNormalAttack::OnFirstSecondMontageEnd()
{
	GEngine->AddOnScreenDebugMessage(55,10.f,FColor::Green,FString::Printf(TEXT("%d"),CurrentComboCount));
	if (CurrentComboCount==2)
	{
		auto* PlayLastAMTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,FName("LastComboMontageTask"),LastComboMontage,0.75f,NAME_None,true
			,1.f,0.f,true);
		PlayLastAMTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnLastMontageEnd);
		PlayLastAMTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnInterrupted);
		PlayLastAMTask->ReadyForActivation();
		ASC->ForceReplication();
	}
	else
	{
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
	}
}

void UGA_FighterNormalAttack::OnLastMontageEnd()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_FighterNormalAttack::OnInterrupted()
{
	CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
}
