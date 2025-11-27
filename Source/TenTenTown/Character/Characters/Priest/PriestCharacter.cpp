#include "PriestCharacter.h"

#include "AbilitySystemComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Character/ENumInputID.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/GAS/AS/PriestAttributeSet/AS_PriestAttributeSet.h"
#include "Character/GAS/GA/Priest/DivineBlessing/GA_Priest_DivineBlessing.h"
#include "Engine/LocalPlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

APriestCharacter::APriestCharacter()
{
	WandMesh=CreateDefaultSubobject<UStaticMeshComponent>("WandMesh");
	WandMesh->SetupAttachment(GetMesh(), WandAttachSocket);
	WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WandMesh->SetGenerateOverlapEvents(false);
	
	//점프 횟수
	JumpMaxCount = 1;

	PrimaryActorTick.bCanEverTick = true;
}

void APriestCharacter::BeginPlay()
{
	Super::BeginPlay();

	WandMesh->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		WandAttachSocket
	);
	WandMesh->SetSimulatePhysics(false);
	WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WandMesh->SetGenerateOverlapEvents(false);
}

void APriestCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PriestAS = ASC ? Cast<UAS_PriestAttributeSet>(ASC->GetAttributeSet(UAS_PriestAttributeSet::StaticClass())) : nullptr;
}

void APriestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDivineBlessingTargetPreview();
	
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

	static const FString Ctx(TEXT("PriestLevelUp"));

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

void APriestCharacter::UpdateDivineBlessingTargetPreview()
{
	if (!ASC || !ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting)) return;
	
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(ENumInputID::SkillA));
	if (!Spec || !Spec->IsActive()) return;
	
	UGA_Priest_DivineBlessing* BlessingGA = Cast<UGA_Priest_DivineBlessing>(Spec->GetPrimaryInstance());
	if (!BlessingGA) return;
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	
	FVector EyeLoc;
	FRotator EyeRot;
	PC->GetPlayerViewPoint(EyeLoc, EyeRot);

	float MaxRange = 1500.f;
	const FVector Start = EyeLoc + EyeRot.Vector() * 350.f;
	const FVector End = Start + EyeRot.Vector() * MaxRange;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DivinBlessingTrace), false, this);

	float TraceRadius = 100;
	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);
	
	ABaseCharacter* NewTarget = nullptr;
	if (bHit)
	{
		float BestDistSq = TNumericLimits<float>::Max();
		
		for (const FHitResult& H : Hits)
		{
			ABaseCharacter* HitChar = Cast<ABaseCharacter>(H.GetActor());
			if (!HitChar || HitChar == this) continue;
			if (!HitChar->ActorHasTag(TEXT("Playable"))) continue;
			
			const FVector ActorLoc = HitChar->GetActorLocation();
			const FVector ClosestOnLine =
				FMath::ClosestPointOnSegment(ActorLoc, Start, End);

			const float DistSq =
				FVector::DistSquared(ActorLoc, ClosestOnLine);

			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				NewTarget = HitChar;
			}
		}
	}
	BlessingGA->SetPreviewTarget(NewTarget);
}