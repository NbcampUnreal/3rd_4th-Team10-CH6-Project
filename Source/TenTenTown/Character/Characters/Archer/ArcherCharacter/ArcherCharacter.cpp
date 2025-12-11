// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherCharacter.h"

#include "Character/Characters/Archer/Bow/ArcherBow.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

AArcherCharacter::AArcherCharacter()
{
	JumpMaxCount = 2;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	bUseControllerRotationYaw = true;
	PrimaryActorTick.bCanEverTick = true;
}

AArcherBow* AArcherCharacter::GetEquippedBow()
{
	return EquippedBow;
}

void AArcherCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->ConsoleCommand("ShowDebug AbilitySystem 1");
		PC->ConsoleCommand("AbilitySystem.DebugAttribute Health MaxHealth");
	}
	
	EquipBow();
}

void AArcherCharacter::EquipBow()
{
	if (BowClass && !EquippedBow)
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.Instigator = GetInstigator();
		
		EquippedBow = GetWorld()->SpawnActor<AArcherBow>(BowClass,GetActorTransform(),Parameters);
		
		if (EquippedBow)
		{
			EquippedBow->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,FName("Weapon_L"));
		}
	}
}
