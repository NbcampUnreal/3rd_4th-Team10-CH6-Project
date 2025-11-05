// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Fireball.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimSequence.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Fireball::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASC = Cast<ATTTPlayerState>(GetOwningActorFromActorInfo())->GetAbilitySystemComponent();
	AvatarCharacter=Cast<ACharacter>(GetAvatarActorFromActorInfo());
	OriginSpeed = AvatarCharacter->GetCharacterMovement()->GetMaxSpeed();

	
	if (!ASC||!AvatarCharacter)
	{
		UE_LOG(LogTemp,Log,TEXT("no asc or no avatar character"));
		return;
	}
	
	auto* AMTaskFireball = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("AMTaskFireball"),FireballMontage,1.f,"Start");
	
	
	auto* WaitGameplayEventTask_Charging = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fireball_Charging,nullptr,true);
	WaitGameplayEventTask_Charging->EventReceived.AddUniqueDynamic(this,&ThisClass::ActiveLoopGameplayCue);

	auto* WaitGameplayEventTask_Release = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fireball_Release,nullptr,true);
	WaitGameplayEventTask_Release->EventReceived.AddUniqueDynamic(this,&ThisClass::LaunchFireball);
	
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=100.f;
	WaitGameplayEventTask_Charging->ReadyForActivation();
	WaitGameplayEventTask_Release->ReadyForActivation();
	AMTaskFireball->ReadyForActivation();
	ASC->ForceReplication();
}

void UGA_Fireball::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Fireball::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	auto* AMTaskRelease = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("AMTaskRelease"),FireballReleaseMontage,1.5f);
	AMTaskRelease->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnAbilityEnd);
	AMTaskRelease->ReadyForActivation();
	ASC->RemoveGameplayCue(GASTAG::GameplayCue_Fireball_Charging);
	
	ASC->ForceReplication();
}

void UGA_Fireball::OnAbilityEnd()
{
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=OriginSpeed;
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_Fireball::ActiveLoopGameplayCue(const FGameplayEventData Data)
{
	ASC->AddGameplayCue(GASTAG::GameplayCue_Fireball_Charging);
}

void UGA_Fireball::LaunchFireball(const FGameplayEventData Data)
{
	// TODO: 여기서부터 시작해서 라인트레이스 및 파이어볼 발사 로직 처리 하기
	UE_LOG(LogTemp,Log,TEXT("FIRE"));
}

