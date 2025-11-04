// TTTGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TTTGameStateBase.generated.h"

UENUM(BlueprintType)
enum class ETTTGamePhase:uint8
{
	Waiting,
	Build,
	Combat,
	Reward,
	Victory,
	GameOver,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTTTOnPhaseChanged,ETTTGamePhase,NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTTTOnRemainingTimeChanged,int32,NewReamaining);
UCLASS()
class TENTENTOWN_API ATTTGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATTTGameStateBase();

	UPROPERTY(ReplicatedUsing=OnRep_Phase, BlueprintReadOnly, Category="TTT|Phase")
	ETTTGamePhase Phase = ETTTGamePhase::Waiting;

	UPROPERTY(ReplicatedUsing=OnRep_RemainingTime, BlueprintReadOnly, Category="TTT|Phase")
	int32 RemainingTime = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="TTT|Phase")
	int32 Wave = 1;

	UPROPERTY(BlueprintAssignable, Category="TTT|Phase")
	FTTTOnPhaseChanged OnPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category="TTT|Phase")
	FTTTOnRemainingTimeChanged OnRemainingTimeChanged;
	
	UFUNCTION()
	void OnRep_Phase();

	UFUNCTION()
	void OnRep_RemainingTime();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
