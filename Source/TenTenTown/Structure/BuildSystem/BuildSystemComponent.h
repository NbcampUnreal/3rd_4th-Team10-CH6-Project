#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "BuildSystemComponent.generated.h"

class UInputMappingContext;
class UAbilitySystemComponent;
class UInputAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TENTENTOWN_API UBuildSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildSystemComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 빌드 모드 IMC
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Build System")
	TObjectPtr<UInputMappingContext> IMC_Build;

	// 현재 바라보고 있는 구조물
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Build System")
	TObjectPtr<AActor> HoveredStructure;

	// 빌드 모드 누르면
	void ToggleBuildMode();
	// 구조물 선택
	void SelectStructure(int32 SlotIndex);
	// 확인, 업그레이드
	void HandleConfirmAction();
	// 취소, 파괴
	void HandleCancelAction();
	// 수리
	void HandleRepairAction();

protected:
	// 라인 트레이스
	void TickBuildModeTrace();

	// 서버 RPC
	UFUNCTION(Server, Reliable)
	void Server_InteractStructure(AActor* TargetActor, FGameplayTag InteractionTag);

	// ASC 가져오기
	UAbilitySystemComponent* GetOwnerASC() const;
};
