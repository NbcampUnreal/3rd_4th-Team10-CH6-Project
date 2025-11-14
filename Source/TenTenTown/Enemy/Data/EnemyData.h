// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enemy/Base/EnemyBase.h"
#include "EnemyData.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Melee,
	Ranged,
	Boss
};

USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EnemyName = "a";//적 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyType Type = EEnemyType::Melee;//적 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveNum = 0;//스폰되는 웨이브
	
	//Attribute Set에 적용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Attack = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 100.0f;
	
	//처치 시 골드,경험치 보상
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gold = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EXP = 10;

	//에셋 경로
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> EnemyBP;

};
