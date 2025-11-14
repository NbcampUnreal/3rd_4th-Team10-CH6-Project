// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ENumInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Jump UMETA(DisplayName = "Jump"),
	Dash UMETA(DisplayName = "Dash"),
	Sprint UMETA(DisplayName = "Sprint"),
	SkillA UMETA(DisplayName = "SkillA"),
	SkillB UMETA(DisplayName = "SkillB"),
	Ult UMETA(DisplayName = "Ult"),
	UltimateNormalAttack UMETA(DisplayName="Ultimate Normal Attack"),
	NormalAttack UMETA(DisplayName = "Noraml Attack"),
	ChargeAttack UMETA(DisplayName = "Charge Attack"),
	ComboAttack UMETA(DisplayName = "Combo Attack"),
	RightChargeAttack UMETA(DisplayName ="RightChargeAttack"),
	InstallStructure UMETA(DisplayName = "Install Structure"),
	CancelStructure UMETA(DisplayName = "Cencle Structure")
};