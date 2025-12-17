#include "GA_PotionItem.h"

#include "AbilitySystemComponent.h"

UGA_PotionItem::UGA_PotionItem()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Item.DrinkPotion"));
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_PotionItem::ApplyOnServer(const FGameplayEventData& Payload)
{
	if (!ASC) return;
	if (ItemData.PassiveEffect)
	{
		FGameplayTag CueTag;

		if (ItemData.ItemTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Data.Item.Potion.HP"))))
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Potion.HP"));
		}
		else if (ItemData.ItemTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Data.Item.Potion.MP"))))
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Potion.MP"));
		}
		else
		{
			CueTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Item.Apply"));
		}
		
		FGameplayCueParameters CueParams;
		CueParams.Location = CurrentActorInfo->AvatarActor.Get()->GetActorLocation();
		ASC->ExecuteGameplayCue(CueTag, CueParams);
		
		FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
		Ctx.AddSourceObject(this);

		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(ItemData.PassiveEffect, 1.f, Ctx);
		Spec.Data->SetSetByCallerMagnitude(ItemData.ItemTag, ItemData.Magnitude);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}


