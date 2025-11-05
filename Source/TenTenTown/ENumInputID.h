// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ENumInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Jump UMETA(DisplayName = "Jump"),
	Dash UMETA(DisplayName = "Dash"),
	ComboAttack UMETA(DisplayName = "Combo Attack"),
	ChargeAttack UMETA(DisplayName = "Charge Attack"),
	RightChargeAttack UMETA(DisplayName ="RightChargeAttack")
};