//TTTGameModeBase.h


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTTGameStateBase.h"
#include "TTTGameModeBase.generated.h"


UCLASS()
class TENTENTOWN_API ATTTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATTTGameModeBase();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "TTT|Phase")
	void StartPhase(ETTTGamePhase NewPhase, int32 DurationSeconds);

	UPROPERTY(EditAnywhere, Category="TTT|Game") int32 MaxWaves = 3;
	void EndGame(bool bVictory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* EnemyDataTableAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> WaveDataTableAsset;

	void SetupDataTables();

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;


protected:
	FTimerHandle TimerHandle_Tick1s;

	void TickPhaseTimer();
	void AdvancePhase();
	int32 GetDefaultDurationFor(ETTTGamePhase Phase) const;

	UFUNCTION(Exec)
	void PM_SetPhase(const FString& Name);

private:
	ATTTGameStateBase* GS() const { return GetGameState<ATTTGameStateBase>(); };
	
};
