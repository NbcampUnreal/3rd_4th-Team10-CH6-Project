// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Whirlwind.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Whirlwind::UGA_Whirlwind()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bReplicateInputDirectly = true;
	bAlreadyEnd=false;
}

void UGA_Whirlwind::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	bAlreadyEnd=false;
	
	auto* PlayWhirlWindStartTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("PlayWhirlWindStartTask"),WhirlWindMontage,1.f,NAME_None,true,
		1.f,0.f,true);
	PlayWhirlWindStartTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnEnd);
	PlayWhirlWindStartTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnFirstMontageEnd);
	
	auto* DurationEndTask = UAbilityTask_WaitDelay::WaitDelay(this,Duration);
	DurationEndTask->OnFinish.AddUniqueDynamic(this,&ThisClass::UGA_Whirlwind::OnDurationEnd);
	
	auto* WaitGameEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		 this,GASTAG::Event_Fighter_WhirlwindAttack,nullptr,false,true);
	WaitGameEventTask->EventReceived.AddUniqueDynamic(this,&ThisClass::OnAttack);
	
	PlayWhirlWindStartTask->ReadyForActivation();
	DurationEndTask->ReadyForActivation();
	WaitGameEventTask->ReadyForActivation();
	
	RotateStartTime=GetWorld()->GetTimeSeconds();
}

void UGA_Whirlwind::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	CommitAbility(Handle,ActorInfo,ActivationInfo);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Whirlwind::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (bAlreadyEnd) return;
	
	bAlreadyEnd=true;
	
	RotateEndTime = GetWorld()->GetTimeSeconds();
	SpinTime= RotateEndTime-RotateStartTime;
	
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,
		FString::Printf(TEXT("Spin Time: %f"),SpinTime));
	if (SpinTime>3.f)
	{
		ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=0.f;
		
		auto* DizzyAnimMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,FName("DizzyAnimMontage"),DizzyMontage,1.f,NAME_None,
			true,1.f,0.f,true);
		DizzyAnimMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnDizzyEnd);
		DizzyAnimMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnDizzyEnd);
		
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(GASTAG::State_Fighter_Dizzy);
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(GASTAG::State_Block_Everything);
		DizzyAnimMontageTask->ReadyForActivation();
		
	}
	else
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Whirlwind::OnEnd()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_Whirlwind::OnFirstMontageEnd()
{
	auto* WhirlWindLoopMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,FName("LoopMontageTask"),WhirlWindLoopMontage,1.f,"Start",true
	,1.f,0.f,true);
	WhirlWindLoopMontageTask->ReadyForActivation();
}

void UGA_Whirlwind::OnDurationEnd()
{
	if (bAlreadyEnd) return;
	
	bAlreadyEnd=true;
	
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=0.f;
		
	auto* DizzyAnimMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("DizzyAnimMontage"),DizzyMontage,1.f,NAME_None,
		true,1.f,0.f,true);
	DizzyAnimMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnDizzyEnd);
	DizzyAnimMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnDizzyEnd);
		
	DizzyAnimMontageTask->ReadyForActivation();
	GetAbilitySystemComponentFromActorInfo()->ForceReplication();
}

void UGA_Whirlwind::OnDizzyEnd()
{
	
	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=300.f;
	
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(GASTAG::State_Fighter_Dizzy);
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(GASTAG::State_Block_Everything);
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_Whirlwind::OnAttack(const FGameplayEventData Payload)
{
}