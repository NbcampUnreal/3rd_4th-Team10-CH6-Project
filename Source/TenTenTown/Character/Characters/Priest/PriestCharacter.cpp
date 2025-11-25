#include "PriestCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Character/ENumInputID.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/GAS/AS/PriestAttributeSet/AS_PriestAttributeSet.h"
#include "Engine/LocalPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

APriestCharacter::APriestCharacter()
{
	//점프 횟수
	JumpMaxCount = 1;

	PrimaryActorTick.bCanEverTick = true;
}

void APriestCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PriestAS = ASC ? Cast<UAS_PriestAttributeSet>(ASC->GetAttributeSet(UAS_PriestAttributeSet::StaticClass())) : nullptr;
}

void APriestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!PriestAS) return;
	
	const float M  = PriestAS->GetMana();
	const float MM = PriestAS->GetMaxMana();
	
	const FString Msg = FString::Printf(TEXT("Mana %.0f/%.0f"), M, MM);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, Msg);
}

void APriestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(SkillAAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillA);
		EIC->BindAction(SkillBAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillB);
		EIC->BindAction(UltAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
		EIC->BindAction(UltAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
		EIC->BindAction(UltAction, ETriggerEvent::Canceled, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
	}
}

void APriestCharacter::RecalcStatsFromLevel(float NewLevel)
{
	Super::RecalcStatsFromLevel(NewLevel);
	
	if (!LevelUpCurveTable || !ASC || !PriestAS) return;

	static const FString Ctx(TEXT("MageLevelUp"));

	auto EvalRow = [&](FName RowName, float& OutValue)
	{
		if (const FRealCurve* Curve = LevelUpCurveTable->FindCurve(RowName, Ctx))
			OutValue = Curve->Eval(NewLevel);
		else
			OutValue = 0.f;
	};

	float NewMaxMana = 0.f;
	
	EvalRow(TEXT("MaxMana"), NewMaxMana);
	
	ASC->SetNumericAttributeBase(UAS_PriestAttributeSet::GetMaxManaAttribute(), NewMaxMana);
	ASC->SetNumericAttributeBase(UAS_PriestAttributeSet::GetManaAttribute(), NewMaxMana);
}