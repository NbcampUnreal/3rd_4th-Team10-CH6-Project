#include "DemonKing.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"

ADemonKing::ADemonKing()
{
	bBerserkPlayed = false;
}


void ADemonKing::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_DemonKing);
	}
		
}
void ADemonKing::ResetEnemy()
{
	Super::ResetEnemy();
	bBerserkPlayed = false;

}
