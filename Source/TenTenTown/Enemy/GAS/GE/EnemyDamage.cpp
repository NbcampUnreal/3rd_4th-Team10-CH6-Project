// EnemyDamage.cpp
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "EnemyDamage.h"

UEnemyDamage::UEnemyDamage()
{
	// GameplayEffect가 Instant로 실행되도록 설정
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Modifier 추가: Damage Attribute에 SetByCaller 값 적용
	FGameplayModifierInfo ModifierInfo;

	ModifierInfo.Attribute = UAS_EnemyAttributeSetBase::GetDamageAttribute();
	ModifierInfo.ModifierOp = EGameplayModOp::Override;

	// SetByCaller 사용
	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Enemy.Damage"));

	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	// Modifier 배열에 추가
	Modifiers.Add(ModifierInfo);
}
