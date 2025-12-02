//TTTGameModeBase.h


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTTGameStateBase.h"
#include "TTTGameModeBase.generated.h"

class ACoreStructure;

UCLASS()
class TENTENTOWN_API ATTTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATTTGameModeBase();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "TTT|Phase")
	void StartPhase(ETTTGamePhase NewPhase, int32 DurationSeconds);
	
	UPROPERTY(EditAnywhere, Category="TTT|Game") int32 MaxWaves = 1;
	void EndGame(bool bVictory);

	void ReturnToLobby();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable*  WaveDataTableAsset;

	void SetupDataTables();

	// ====== 네트워크/플레이어 관련 ======
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
protected:
	APawn* SpawnSelectedCharacter(AController* NewPlayer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Core")
	ACoreStructure* CoreStructure = nullptr;

	
	FTimerHandle TimerHandle_Tick1s;

	void TickPhaseTimer();
	void AdvancePhase();
	int32 GetDefaultDurationFor(ETTTGamePhase Phase) const;

	UFUNCTION(Exec)
	void PM_SetPhase(const FString& Name);

	void CheckAllCharactersSpawnedAndStartBuild();

	void BindCoreEvents();

	UFUNCTION()
	void HandleCoreDead();   // OnDead 델리게이트용


private:
	ATTTGameStateBase* GS() const { return GetGameState<ATTTGameStateBase>(); };

	bool bHasReturnedToLobby = false;
	


#pragma region UI_Region
public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void InitializeAllPlayerStructureLists();

protected:
	/*TArray<FInventoryItemData> CreateInitialStructureList(UDataTable* DataTable);*/
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> PlayStateGEClass;
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> CharSelectGEClass;*/
#pragma endregion



};
