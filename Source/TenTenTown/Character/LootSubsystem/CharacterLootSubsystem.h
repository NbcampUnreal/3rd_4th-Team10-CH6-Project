// CharacterLootSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CharacterLootSubsystem.generated.h"

class ATTTGameStateBase;

UCLASS(BlueprintType)
class TENTENTOWN_API UCharacterLootSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- Getter (읽기 전용) ---
	UFUNCTION(BlueprintPure, Category = "Loot")
	int32 GetGold();

	UFUNCTION(BlueprintPure, Category = "Loot")
	float GetEXP();

	// --- Setter / Adder (서버 로직) ---
	UFUNCTION(BlueprintCallable, Category = "Loot")
	void SetGold(int32 NewGold);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void SetEXP(float NewEXP);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void AddGold(int32 PlusGold);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void AddEXP(float PlusEXP);

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void SetZero();

private:
	// 내부 헬퍼 함수
	ATTTGameStateBase* GetGameState();
	bool IsServerSide();
};