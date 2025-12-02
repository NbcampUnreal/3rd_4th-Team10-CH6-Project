// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/AS/BlackKnight_AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"

void UBlackKnight_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 공격자 위치 기반 전방 판정
		if (AActor* Attacker = Cast<AActor>(Data.EffectSpec.GetContext().GetEffectCauser()))
		{
			FVector Forward = GetOwningActor()->GetActorForwardVector();
			FVector ToAttacker = (Attacker->GetActorLocation() - GetOwningActor()->GetActorLocation()).GetSafeNormal();

			float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Forward, ToAttacker)));
			const float FrontAngle = 180.f;

			if (Angle <= FrontAngle / 2.f)
			{
				
				float ReducedDamage = GetDamage() * 0.1f; //전방에서 공격 시 데미지 90% 감소
				SetDamage(ReducedDamage);
			}
		}
	}
	Super::PostGameplayEffectExecute(Data);

}
