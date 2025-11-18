#include "DemonKing.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "AbilitySystemComponent.h"
ADemonKing::ADemonKing()
{
	PrimaryActorTick.bCanEverTick = true;
	bBerserkPlayed = false;
}

//현재 tick으로 테스트
void ADemonKing::CheckBerserkState()
{
	if (!ASC || !EnemyBerserk) return;

	if (const UAS_EnemyAttributeSetBase* AttrSet = ASC->GetSet<UAS_EnemyAttributeSetBase>())
	{
		float HealthRatio = AttrSet->GetHealth() / AttrSet->GetMaxHealth();

		if (HealthRatio <= BerserkHealthThreshold)
		{
			
			if (!ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Berserk))
			{
				ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Berserk);

				FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
				EffectContext.AddInstigator(this, this);

				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EnemyBerserk, 1.0f, EffectContext);
				if (SpecHandle.IsValid())
				{
					ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
		if(HealthRatio == 0.0f)
		{
			ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Dead);

		}
	}
}


void ADemonKing::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ASC)
	{
		ASC->AddLooseGameplayTag(GASTAG::Enemy_Type_DemonKing);
	}
		
}
void ADemonKing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckBerserkState();
}
