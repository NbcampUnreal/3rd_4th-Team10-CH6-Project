#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataTable.h"
#include "GA_InstallStructure.generated.h"

UCLASS()
class TENTENTOWN_API UGA_InstallStructure : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_InstallStructure();

protected:
	// 데이터 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Structure")
	FDataTableRowHandle StructureDataRow;

	// 현재 떠 있는 프리뷰 액터 저장
	UPROPERTY()
	TObjectPtr<AActor> PreviewActor;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 이벤트
	UFUNCTION()
	void OnConfirm(FGameplayEventData Payload);
	UFUNCTION()
	void OnCancel(FGameplayEventData Payload);
};
