// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AArcherCharacter::AArcherCharacter()
{
	JumpMaxCount = 2;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	PrimaryActorTick.bCanEverTick = true;
}
