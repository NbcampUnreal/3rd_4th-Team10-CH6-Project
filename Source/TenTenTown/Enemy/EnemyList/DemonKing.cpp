#include "DemonKing.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"


void ADemonKing::ResetEnemy()
{
	Super::ResetEnemy();
	bBerserkPlayed = false;

}
void ADemonKing::InitializeEnemy()
{
	Super::InitializeEnemy();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_DemonKing);
	}
}
