#include "Structure/Core/AS/AS_CoreAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAS_CoreAttributeSet::UAS_CoreAttributeSet()
{
	InitHealth(20.0f);
	InitMaxHealth(20.0f);
}

void UAS_CoreAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 어떤 어트리뷰트가 변경되었는지 확인합니다.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Health 값을 0과 MaxHealth 사이로 강제합니다 (클램핑).
		float NewHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
		SetHealth(NewHealth);

		if (GetOwningActor() && GetOwningActor()->GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("AS_CoreAttributeSet - 코어 체력 변경됨: %f / %f"), NewHealth, GetMaxHealth());
		}
		
		// 체력이 0 이하가 되었는지 확인합니다.
		if (NewHealth <= 0.0f)
		{
			// TODO: 2번 요구사항 (게임모드로 체력 0 신호 보내기)
			// 이 코드는 서버에서만 실행되어야 합니다.
			AActor* OwnerActor = GetOwningActor();
			if (OwnerActor && OwnerActor->GetLocalRole() == ROLE_Authority)
			{
				// 예: AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode());
				// if (GameMode)
				// {
				// 	GameMode->OnCoreDied(); // 게임모드에 만든 커스텀 함수 호출
				// }
			}
		}
	}
}

void UAS_CoreAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Health와 MaxHealth를 모든 클라이언트에 복제합니다.
	DOREPLIFETIME_CONDITION_NOTIFY(UAS_CoreAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAS_CoreAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UAS_CoreAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_CoreAttributeSet, Health, OldHealth);
}

void UAS_CoreAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAS_CoreAttributeSet, MaxHealth, OldMaxHealth);
}
