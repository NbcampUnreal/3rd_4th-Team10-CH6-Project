#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Enemy/EnemyList/DemonKing.h"
#include "Engine/TimerHandle.h"
#include "SPAttackTask.generated.h"

UCLASS()
class TENTENTOWN_API USPAttackTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

private:
	FTimerHandle SPAttackTimerHandle;

	// SP 공격 재생 속도
	float AttackSpeed = 1.f;

	// 버서크 몽타주 재생 여부
	bool bHasPlayedBerserkMontage = false;

	// 공격 로직
	void ExecuteSPAttack();
	void ExecuteRotate();

public:
	// 공격 대상과 액터 참조
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Context))
	ADemonKing* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Input))
	AActor* TargetActor;

	// SP 공격 딜레이
	UPROPERTY(EditAnywhere, Category="Boss|Attack")
	float SPAttackDelay = 0.5f;

	
	// StateTree 진입/종료 오버라이드
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
										   const FStateTreeTransitionResult& Transition) override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
						   const FStateTreeTransitionResult& Transition) override;
};
