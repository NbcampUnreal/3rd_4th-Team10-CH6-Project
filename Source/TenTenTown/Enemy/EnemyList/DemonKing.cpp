#include "DemonKing.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"

ADemonKing::ADemonKing()
{
	bBerserkPlayed = false;
	//PrimaryActorTick.bCanEverTick = true;
}

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

void ADemonKing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ADemonKing::BeginPlay()
{
	Super::BeginPlay();
	
}
