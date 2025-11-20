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
	
	UPROPERTY(EditAnywhere, Category="TTT|Game") int32 MaxWaves = 6;
	void EndGame(bool bVictory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* EnemyDataTableAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> WaveDataTableAsset;

	void SetupDataTables();

	// ====== 네트워크/플레이어 관련 ======
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
protected:
	APawn* SpawnSelectedCharacter(AController* NewPlayer);
	
	FTimerHandle TimerHandle_Tick1s;

	void TickPhaseTimer();
	void AdvancePhase();
	int32 GetDefaultDurationFor(ETTTGamePhase Phase) const;

	UFUNCTION(Exec)
	void PM_SetPhase(const FString& Name);

private:
	ATTTGameStateBase* GS() const { return GetGameState<ATTTGameStateBase>(); };
	
};
