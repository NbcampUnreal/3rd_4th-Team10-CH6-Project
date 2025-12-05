#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "AbilitySystemComponent.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "UI/PCC/PlayPCComponent.h"
#include "PlayerStatusViewModel.generated.h"

class ATTTPlayerState;

UCLASS()
class TENTENTOWN_API UPlayerStatusViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	UPlayerStatusViewModel();

	// PlayerController Component에서 호출하여 초기화 및 구독을 설정하는 함수
	void InitializeViewModel(UPlayPCComponent* PlayPCC, ATTTPlayerState* PS, UAbilitySystemComponent* ASC);

	// PC Component 종료 시 구독을 해제하고 정리하는 함수
	void CleanupViewModel();

	// GAS Attribute 콜백 함수들
	void OnLevelChanged(const FOnAttributeChangeData& Data);
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnManaChanged(const FOnAttributeChangeData& Data);
	void OnMaxManaChanged(const FOnAttributeChangeData& Data);
	//void OnGoldChanged(int32 NewGold);


	// **레벨 및 경험치 관련 속성**

	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
	float ExpPercentage = 0.0f;

	// **체력 및 스태미나/마나 관련 속성**

	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
	float HealthPercentage = 1.0f;

	// NOTE: Mage AS에 Stamina가 없으므로 Mana 비율을 표시하는 용도로 사용합니다.
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
	float ManaPercentage = 1.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter)
	int32 PlayerGold = 0;

protected:
	// --- 내부 데이터 저장소 ---

	// GAS에서 현재 값을 저장하고 백분율 계산에 사용
	float CurrentHealth = 1.0f;
	float MaxHealth = 1.0f;
	float CurrentMana = 1.0f;
	float MaxMana = 1.0f;

	

	// 캐시된 데이터 소스
	UPROPERTY()
	TObjectPtr<ATTTPlayerState> CachedPlayerState;
	UPROPERTY()
	TObjectPtr<UInventoryPCComponent> CachedInventory;

	UPROPERTY()
	TObjectPtr<UPlayPCComponent> CachedPlayPCComponent;

	// --- 백분율 재계산을 위한 내부 함수 ---
	void RecalculateHealthPercentage();
	void RecalculateManaPercentage();


	// --- Getter & Setter 구현 (FieldNotify) ---
public:
	// 레벨
	UFUNCTION()
	int32 GetLevel() const { return Level; }
	UFUNCTION()
	void SetLevel(int32 NewLevel); // 구현은 cpp에서

	// 경험치 백분율
	UFUNCTION()
	float GetExpPercentage() const { return ExpPercentage; }
	UFUNCTION()
	void SetExpPercentage(float NewPercentage);

	// 체력 백분율
	UFUNCTION()
	float GetHealthPercentage() const { return HealthPercentage; }
	UFUNCTION()
	void SetHealthPercentage(float NewPercentage);

	// 스태미나/마나 백분율
	UFUNCTION()
	float GetManaPercentage() const { return ManaPercentage; }
	UFUNCTION()
	void SetManaPercentage(float NewPercentage);

	UFUNCTION()
	int32 GetPlayerGold() const { return PlayerGold; }
	UFUNCTION()
	void SetPlayerGold(int32 NewGold);
	

	void OnOffTraderWindow(bool OnOff);
};