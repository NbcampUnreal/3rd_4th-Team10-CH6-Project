// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/BlackKnight_AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UBlackKnight_AttributeSet::UBlackKnight_AttributeSet()
{
	InitFrontHitCount(0.f);
}

void UBlackKnight_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 공격자 위치 기반 전방 판정
		if (AActor* Attacker = Cast<AActor>(Data.EffectSpec.GetContext().GetEffectCauser()))
		{
			FVector Forward = GetOwningActor()->GetActorForwardVector();
			FVector ToAttacker = (Attacker->GetActorLocation() - GetOwningActor()->GetActorLocation()).GetSafeNormal();

			float Dot = FVector::DotProduct(Forward, ToAttacker);

			if (Dot>=0.f)//전방 180도
			{
				
				float ReducedDamage = GetDamage() * 0.1f; //전방에서 공격 시 데미지 90% 감소
				SetDamage(ReducedDamage);

				float OldCount = FrontHitCount.GetCurrentValue(); 
				float NewCount = FMath::Clamp(OldCount + 1.f, 0.f, MaxFrontHitCount);
				SetFrontHitCount(NewCount);

				UE_LOG(LogTemp, Warning, TEXT("FrontHitCount=%f"), NewCount);

				UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
				if (NewCount == MaxFrontHitCount && ASC->GetOwnerRole() == ROLE_Authority)
				{
					FGameplayTag CounterTag = FGameplayTag::RequestGameplayTag(FName("Enemy.State.Counter"));
					if (!ASC->HasMatchingGameplayTag(CounterTag))
					{
						ASC->AddLooseGameplayTag(CounterTag);
					}
				}
				if (OldCount < MaxFrontHitCount)
				{
					FGameplayTag GuardCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Enemy.Effect.Guard"));
					FGameplayCueParameters Params;
					ASC->ExecuteGameplayCue(GuardCueTag, Params);
				}
			}
		}
	}
	Super::PostGameplayEffectExecute(Data);

}

void UBlackKnight_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, FrontHitCount, COND_None, REPNOTIFY_Always);

}

void UBlackKnight_AttributeSet::OnRep_FrontHitCount(const FGameplayAttributeData& OldFrontHitCount)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,FrontHitCount,OldFrontHitCount);
}
