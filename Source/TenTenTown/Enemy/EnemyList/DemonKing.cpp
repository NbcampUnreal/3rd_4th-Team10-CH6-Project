#include "DemonKing.h"
#include "Enemy/GAS/AS/AS_EnemyAttributeSetBase.h"
#include "AbilitySystemComponent.h"
ADemonKing::ADemonKing()
{
	PrimaryActorTick.bCanEverTick = true;
}

//추후 피격 로직 구현 시 적용, 현재는 tick으로 테스트
void ADemonKing::CheckBerserkState()
{
	if (ASC)
	{
		if (const UAS_EnemyAttributeSetBase* AttrSet = ASC->GetSet<UAS_EnemyAttributeSetBase>())
		{
			float HealthRatio = AttrSet->GetHealth() / AttrSet->GetMaxHealth();
			if (HealthRatio <= BerserkHealthThreshold)
			{
				if (!ASC->HasMatchingGameplayTag(GASTAG::Enemy_State_Berserk))
				{
					ASC->AddLooseGameplayTag(GASTAG::Enemy_State_Berserk);
					if (BerserkMontage)
					{
						PlayMontage(BerserkMontage, FMontageEnded(), 1.0f);
						Multicast_PlayMontage(BerserkMontage, 1.0f);
						UE_LOG(LogTemp, Warning, TEXT("Berserk!"));
					}

				}
				
			}
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
