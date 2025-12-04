// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ENumInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Jump UMETA(DisplayName = "Jump"),
	Sprint UMETA(DisplayName = "Sprint"),
	Dash UMETA(DisplayName = "Dash"),
	
	NormalAttack UMETA(DisplayName = "Noraml Attack"),
	SkillA UMETA(DisplayName = "SkillA"),
	SkillB UMETA(DisplayName = "SkillB"),
	Ult UMETA(DisplayName = "Ult"),
	
	ChargeAttack UMETA(DisplayName = "Charge Attack"),
	ComboAttack UMETA(DisplayName = "Combo Attack"),
	UltimateNormalAttack UMETA(DisplayName="Ultimate Normal Attack"),
	RightChargeAttack UMETA(DisplayName ="RightChargeAttack"),
	
	InstallStructure UMETA(DisplayName = "Install Structure"),

	// 빌드 모드
	ToggleBuildMode, 
    
	SelectStructure1,
	SelectStructure2,
	SelectStructure3,
	SelectStructure4,
	SelectStructure5,
	SelectStructure6,
};