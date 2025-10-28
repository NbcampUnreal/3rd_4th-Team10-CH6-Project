// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/PS/TTTPlayerState.h"

// Sets default values
AFighterCharacter::AFighterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFighterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFighterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	//서버에서 점유될 때
	PS = Cast<ATTTPlayerState>(GetPlayerState());
	ASC = PS->GetAbilitySystemComponent();

	for (const auto& IDnAbility : InputIDGAMap)
	{
		const auto& [InputID,Ability]=IDnAbility;
		FGameplayAbilitySpec Spec(Ability,1,static_cast<int32>(InputID));
		ASC->GiveAbility(Spec);
	}

	ASC->InitAbilityActorInfo(PS,this);
}

void AFighterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

// Called every frame
void AFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

