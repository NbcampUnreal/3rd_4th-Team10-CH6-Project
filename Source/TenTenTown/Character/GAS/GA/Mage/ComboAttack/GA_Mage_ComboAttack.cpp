#include "GA_Mage_ComboAttack.h"

#include "Animation/AnimMontage.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Mage/MageCharacter.h"

UGA_Mage_ComboAttack::UGA_Mage_ComboAttack()
{
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Mage.Attack")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Combo")));
}

void UGA_Mage_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bool bAnyMontage = false;
	for (int32 i = 0; i < ComboAttackAM.Num(); i++)
	{
		if (ComboAttackAM[i])
		{
			bAnyMontage = true;
		}
	}
	if (!bAnyMontage || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ComboIdx = 0;
	bWindowOpen = false;
	bWantsNext = false;

	BindEvents();
	PlayMontage();
	ScheduleReset();
}

void UGA_Mage_ComboAttack::BindEvents()
{
	WaitOpen = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OpenTag, nullptr, false, true);
	WaitOpen->EventReceived.AddDynamic(this, &UGA_Mage_ComboAttack::OnOpen);
	WaitOpen->ReadyForActivation();
	
	WaitClose = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CloseTag, nullptr, false, true);
	WaitClose->EventReceived.AddDynamic(this, &UGA_Mage_ComboAttack::OnClose);
	WaitClose->ReadyForActivation();
	
	WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitTag, nullptr, false, true);
	WaitHit->EventReceived.AddDynamic(this, &UGA_Mage_ComboAttack::OnHit);
	WaitHit->ReadyForActivation();
}

void UGA_Mage_ComboAttack::OnMontageCompleted()
{
	if (!IsActive()) return;
	const bool bIsLast = (ComboIdx >= 1);
	if (bIsLast && !bWindowOpen && !bWantsNext)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Mage_ComboAttack::OnMontageBlendOut()
{
	OnMontageCompleted();
}

void UGA_Mage_ComboAttack::OnMontageInterrupted()
{
	if (bEndedByMontage || !IsActive()) return;
	bEndedByMontage = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Mage_ComboAttack::OnMontageCancelled()
{
	if (bEndedByMontage || !IsActive()) return;
	bEndedByMontage = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Mage_ComboAttack::PlayMontage()
{
	
	
	if (!CurrentActorInfo) return;
	bEndedByMontage = false;

	if (PlayTask)
	{
		PlayTask->EndTask(); PlayTask = nullptr;
	}
	
	UAnimMontage* AM = ComboAttackAM[FMath::Clamp(ComboIdx, 0, 1)];
	if (!AM)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM, 1.f, NAME_None, false);
	PlayTask->OnInterrupted.AddDynamic(this, &UGA_Mage_ComboAttack::OnMontageInterrupted);
	PlayTask->OnCancelled.AddDynamic(this, &UGA_Mage_ComboAttack::OnMontageCancelled);
	PlayTask->OnCompleted.AddDynamic(this, &UGA_Mage_ComboAttack::OnMontageCompleted);
	PlayTask->OnBlendOut.AddDynamic(this, &UGA_Mage_ComboAttack::OnMontageBlendOut);
	PlayTask->ReadyForActivation();
}

void UGA_Mage_ComboAttack::OnOpen(FGameplayEventData Payload)
{
	bWindowOpen = true;
}

void UGA_Mage_ComboAttack::OnClose(FGameplayEventData Payload)
{
	bWindowOpen = false;
	if (bWantsNext)
	{
		TryAdvance();
	}
	else
	{
		if (ComboIdx >= 1)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}

void UGA_Mage_ComboAttack::OnHit(FGameplayEventData Payload)
{
	DoTraceAndApply();
}

void UGA_Mage_ComboAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bWindowOpen)
	{
		bWantsNext = true;
	}
}

void UGA_Mage_ComboAttack::TryAdvance()
{
	bWantsNext = false;
	ClearReset();

	if (ComboIdx < 1)
	{
		ComboIdx++;
		PlayMontage();
		ScheduleReset();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Mage_ComboAttack::ScheduleReset()
{
	if (!CurrentActorInfo || !CurrentActorInfo->AvatarActor.IsValid()) return;

	CurrentActorInfo->AvatarActor->GetWorldTimerManager().SetTimer(
		ComboResetTimer,
		[this]()
		{
			if (IsActive())
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo,CurrentActivationInfo, true, false);
			}
		},
		ComboTimeOut,
		false
	);
}

void UGA_Mage_ComboAttack::ClearReset()
{
	if (!CurrentActorInfo || !CurrentActorInfo->AvatarActor.IsValid()) return;
	CurrentActorInfo->AvatarActor->GetWorldTimerManager().ClearTimer(ComboResetTimer);
}

void UGA_Mage_ComboAttack::DoTraceAndApply()
{
	// 대미지 로직
}

void UGA_Mage_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearReset();
	bWindowOpen = false;
	bWantsNext = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

