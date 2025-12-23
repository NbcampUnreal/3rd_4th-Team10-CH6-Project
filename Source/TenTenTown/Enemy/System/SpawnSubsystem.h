// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enemy/Data/WaveData.h"
#include "Subsystems/WorldSubsystem.h"
#include "SpawnSubsystem.generated.h"

class ASpawnPoint;
class AEnemyBase;

USTRUCT()
struct FSpawnTask
{
	GENERATED_BODY()
	FEnemySpawnInfo Info; // 스폰할 몬스터 정보
	int32 SpawnedCount = 0; // 지금까지 스폰된 수
	int32 WaveIndex = 0; //스폰되는 웨이브
	FTimerHandle TimerHandle; //반복 호출용 타이머 핸들
	FSpawnTask() {}
	FSpawnTask(int32 InWaveIndex,const FEnemySpawnInfo& InInfo )  : Info(InInfo), WaveIndex(InWaveIndex){}
};

UCLASS()
class TENTENTOWN_API USpawnSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//게임모드에서 데이터 테이블 받아오기
	void SetupTable(TSoftObjectPtr<UDataTable> InWaveData);
	//웨이브에 맞춰 몬스터 스폰 시작
	void StartWave(int32 WaveIndex);
	//보스 웨이브에서 보스 몬스터 스폰
	void SpawnBoss(int32 WaveIndex);
	//웨이브 종료 시 모든 스폰 타이머 종료
	void EndWave(int32 WaveIndex);
private:
	//몬스터 스폰
	void SpawnEnemy(int32 WaveIndex,const FEnemySpawnInfo& EnemyInfo);
	//월드에서 스폰 포인트 이름으로 위치 찾기
	ASpawnPoint* FindSpawnPointByName(FName PointName);
	//웨이브 데이터 따라 몬스터 스폰 반복, 완료 시 타이머 종료
	void HandleSpawnTick(FSpawnTask* SpawnTask);

	UPROPERTY()
	UDataTable* WaveTable = nullptr;

	TArray<FSpawnTask*> ActiveSpawnTasks;
};
