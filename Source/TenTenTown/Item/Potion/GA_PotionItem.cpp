#include "GA_PotionItem.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "UI/PCC/InventoryPCComponent.h"

UGA_PotionItem::UGA_PotionItem()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.DrinkPotion"));
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_PotionItem::ActivateAbility(
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

	if (ItemData.UseType != EItemUseType::Drink)
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

	WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DrinkTag, nullptr, true, true);
	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnDrinkEvent);
	WaitTask->ReadyForActivation();
}

void UGA_PotionItem::OnDrinkEvent(const FGameplayEventData Payload)
{
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;
	if (!ASC) return;

	if (ItemData.PassiveEffect)
	{
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(this);

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(ItemData.PassiveEffect, 1.f, Ctx);
		Spec.Data->SetSetByCallerMagnitude(ItemData.ItemTag, ItemData.Magnitude);
	}

	ConsumeItemOnServer();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
