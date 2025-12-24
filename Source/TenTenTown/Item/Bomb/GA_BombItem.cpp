#include "Item/Bomb/GA_BombItem.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/World.h"
#include "Item/Base/BaseItem.h"

UGA_BombItem::UGA_BombItem()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.Throw"));
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_BombItem::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!InitItem(ActorInfo, TriggerEventData))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ItemData.UseType != EItemUseType::Throw)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ItemData.UseMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ItemData.UseMontage, 1.f, NAME_None, false);
	PlayTask->ReadyForActivation();

	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ThrowTag, nullptr, true, true);
	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnThrowEvent);
	WaitTask->ReadyForActivation();
}

void UGA_BombItem::OnThrowEvent(const FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;
	if (!Char) return;
	if (!ItemData.ItemActorClass) return;

	FActorSpawnParameters Params;
	Params.Owner = Char;
	Params.Instigator = Char;

	const FVector SpawnLoc = Char->GetActorLocation() + Char->GetActorForwardVector() * 50.f;
	const FRotator SpawnRot = Char->GetActorRotation();

	if (ABaseItem* Item = Char->GetWorld()->SpawnActor<ABaseItem>(ItemData.ItemActorClass, SpawnLoc, SpawnRot, Params))
	{
		Item->InitItemData(ItemID, ItemData);
		
		const FVector Dir = Char->GetControlRotation().Vector().GetSafeNormal();
		Item->Throw(Dir);
	}

	ConsumeItemOnServer();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
