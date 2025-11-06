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
	FName EnemyName;//적 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyType Type;//적 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveNum;//스폰되는 웨이브
	
	//Attribute Set에 적용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange;
	
	//처치 시 골드 보상
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gold;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EXP;//경험치 보상

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> EnemyBP;

};
