#include "BaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ENumInputID.h"
#include "TTTGamePlayTags.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Engine/LocalPlayer.h"
#include "Character/InteractionSystemComponent/InteractionSystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/CurveTable.h"

ABaseCharacter::ABaseCharacter()
{
	bUseControllerRotationYaw=true;
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;

	GetCapsuleComponent()->SetCapsuleHalfHeight(70.f);
	
	GetCharacterMovement()->bOrientRotationToMovement=false;
	GetCharacterMovement()->MaxWalkSpeed=600.f;

	SpringArmComponent= CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->bUsePawnControlRotation=true;
	SpringArmComponent->TargetArmLength=300.f;
	SpringArmComponent->SocketOffset = FVector(0.0f,115.f,65.f);
	SpringArmComponent->bDoCollisionTest=true;
	SpringArmComponent->ProbeSize = 10.f;
	SpringArmComponent->ProbeChannel =ECC_Camera;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetupAttachment(SpringArmComponent,USpringArmComponent::SocketName);
	
	//점프 횟수
	JumpMaxCount = 1;
	
	PrimaryActorTick.bCanEverTick = true;
	ISC = CreateDefaultSubobject<UInteractionSystemComponent>("ISC");
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PS = Cast<ATTTPlayerState>(GetPlayerState());
	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
	}

	for (const auto& IDnAbility : InputIDGAMap)
	{
		const auto& [InputID, Ability] = IDnAbility;
		FGameplayAbilitySpec Spec(Ability,1,static_cast<int32>(InputID));
		ASC->GiveAbility(Spec);
	}

	for (const auto& AS : AttributeSets)
	{
		UAttributeSet* AttributeSet = NewObject<UAttributeSet>(PS, AS);
		ASC->AddAttributeSetSubobject(AttributeSet);
	}
	
	CharacterBaseAS = ASC ? Cast<UAS_CharacterBase>(ASC->GetAttributeSet(UAS_CharacterBase::StaticClass())) : nullptr;
	
	for (const TSubclassOf<UGameplayAbility> Passive : PassiveAbilities)
	{
		if (*Passive)
		{
			FGameplayAbilitySpec Spec(Passive, 1, INDEX_NONE, this);
			FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
			
			ASC->TryActivateAbility(Handle);
		}
	}
	
	ASC->InitAbilityActorInfo(PS,this);

	if (HasAuthority() && ASC && CharacterBaseAS)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(CharacterBaseAS->GetLevelAttribute()).AddUObject(this, &ThisClass::OnLevelChanged);
		RecalcStatsFromLevel(CharacterBaseAS->GetLevel());
	}
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PS = Cast<ATTTPlayerState>(GetPlayerState());
	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		CharacterBaseAS = Cast<UAS_CharacterBase>(ASC->GetAttributeSet(UAS_CharacterBase::StaticClass()));
	}
	
	ASC->InitAbilityActorInfo(PS,this);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!CharacterBaseAS) return;
	
	const float H  = CharacterBaseAS->GetHealth();
	const float MH = CharacterBaseAS->GetMaxHealth();
	const float L  = CharacterBaseAS->GetLevel();
	const float A  = CharacterBaseAS->GetBaseAtk();
	
	const FString Msg = FString::Printf(TEXT("HP %.0f/%.0f | LV %.0f | Atk %.0f"), H, MH, L, A);
	if (GEngine) GEngine->AddOnScreenDebugMessage(1001, 0.f, FColor::Cyan, Msg);
}

void ABaseCharacter::GiveDefaultAbility()
{
	if (!ASC || GetLocalRole() != ROLE_Authority) return;

	for (const auto& Pair : InputIDGAMap)
	{
		if (TSubclassOf<UGameplayAbility> GA = Pair.Value)
		{
			const int32 InputID = static_cast<int32>(Pair.Key);
			ASC->GiveAbility(FGameplayAbilitySpec(GA, 1, InputID, this));
		}
	}
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC)
	{
		ULocalPlayer* LP = PC->GetLocalPlayer();
		if (LP)
		{
			UEnhancedInputLocalPlayerSubsystem* EILPS = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			EILPS->AddMappingContext(IMC,0);
		}
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EIC)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		EIC->BindAction(SprintAction,ETriggerEvent::Triggered,this,&ThisClass::ActivateGAByInputID,ENumInputID::Sprint);
		EIC->BindAction(SprintAction,ETriggerEvent::Completed,this,&ThisClass::ActivateGAByInputID,ENumInputID::Sprint);
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Jump);
		EIC->BindAction(DashAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Dash);
		EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::NormalAttack);

		EIC->BindAction(InstallAction,ETriggerEvent::Started,this,&ThisClass::ActivateGAByInputID,ENumInputID::InstallStructure);
		EIC->BindAction(ConfirmAction,ETriggerEvent::Started,this,&ThisClass::ConfirmSelection);
		EIC->BindAction(CancelAction,ETriggerEvent::Started,this,&ThisClass::CancelSelection);

		EIC->BindAction(LevelUpAction, ETriggerEvent::Started, this, &ThisClass::OnLevelUpInput);
	}
}

void ABaseCharacter::Move(const FInputActionInstance& FInputActionInstance)
{
	const FVector Vector = FInputActionInstance.GetValue().Get<FVector>();
	const float ForwardScalar = Vector.X;
	const float RightScalar = Vector.Y;

	FRotator ControllerRotation = GetController()->GetControlRotation();
	ControllerRotation.Pitch=0;

	const FRotationMatrix RotationMatrix(ControllerRotation);
	const FVector ForwardVector = RotationMatrix.GetUnitAxis(EAxis::X);
	const FVector RightVector = RotationMatrix.GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector,ForwardScalar);
	AddMovementInput(RightVector,RightScalar);
}

void ABaseCharacter::Look(const FInputActionInstance& FInputActionInstance)
{
	const FVector InputVector = FInputActionInstance.GetValue().Get<FVector>();
	AddControllerYawInput(InputVector.X);
	AddControllerPitchInput(InputVector.Y);
}

void ABaseCharacter::ConfirmSelection(const FInputActionInstance& FInputActionInstance)
{
	if (!ASC) return;
	
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this,
			GASTAG::Event_Confirm,
			Payload
		);
		
		Server_ConfirmSelection();
	}
}

void ABaseCharacter::CancelSelection(const FInputActionInstance& FInputActionInstance)
{
	if (!ASC) return;
	
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = this;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this,
			GASTAG::Event_Cancel,
			Payload
		);
		
		Server_CancelSelection();
	}
}

void ABaseCharacter::Server_ConfirmSelection_Implementation()
{
	FGameplayEventData Payload;
	Payload.Instigator = this;
		
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		GASTAG::Event_Confirm,
		Payload
	);
}

void ABaseCharacter::Server_CancelSelection_Implementation()
{
	FGameplayEventData Payload;
	Payload.Instigator = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		GASTAG::Event_Cancel,
		Payload
	);
}

void ABaseCharacter::ActivateGAByInputID(const FInputActionInstance& FInputActionInstance, ENumInputID InputID)
{
#if WITH_EDITOR
	if (!ASC)
	{
		UE_LOG(LogTemp,Log,TEXT("NO ASC IN ActivateGAByInputID"));
		return;
	}
#endif
	
	const ETriggerEvent TriggerEvent = FInputActionInstance.GetTriggerEvent();

	if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InputID)))
	{
		switch (TriggerEvent)
		{
		case ETriggerEvent::Started:
		case ETriggerEvent::Triggered:
			if (Spec->IsActive()) ASC->AbilityLocalInputPressed(static_cast<int32>(InputID));
			else ASC->TryActivateAbility(Spec->Handle,true);
			break;
		case ETriggerEvent::Canceled:
		case ETriggerEvent::Completed:
			ASC->AbilityLocalInputReleased(static_cast<int32>(InputID));
			GEngine->AddOnScreenDebugMessage(54,10.f,FColor::Green,TEXT("Released"));
			break;
		case ETriggerEvent::Ongoing:
			break;
		case ETriggerEvent::None:
			break;
		}
	}
}


void ABaseCharacter::OnLevelUpInput(const FInputActionInstance& InputActionInstance)
{
	if (!IsLocallyControlled()) return;
	Server_LevelUp();
}

void ABaseCharacter::Server_LevelUp_Implementation()
{
	if (!ASC || !CharacterBaseAS) return;

	float CurLevel = CharacterBaseAS->GetLevel();
	if (CurLevel >= 10.f) return;

	float NewLevel = CurLevel + 1.f;
	
	ASC->SetNumericAttributeBase(
		UAS_CharacterBase::GetLevelAttribute(),
		NewLevel
	);
}

void ABaseCharacter::OnLevelChanged(const FOnAttributeChangeData& Data)
{
	const float NewLevel = Data.NewValue;
	RecalcStatsFromLevel(NewLevel);
}

void ABaseCharacter::RecalcStatsFromLevel(float NewLevel)
{
	if (!LevelUpCurveTable || !ASC || !CharacterBaseAS) return;

	static const FString Ctx(TEXT("CharacterLevelUp"));

	auto EvalRow = [&](FName RowName, float& OutValue)
	{
		if (const FRealCurve* Curve = LevelUpCurveTable->FindCurve(RowName, Ctx))
			OutValue = Curve->Eval(NewLevel);
		else
			OutValue = 0.f;
	};

	float NewMaxHp   = 0.f;
	float NewBaseAtk = 0.f;

	EvalRow(TEXT("MaxHealth"), NewMaxHp);
	EvalRow(TEXT("BaseAtk"),   NewBaseAtk);
	
	ASC->SetNumericAttributeBase(UAS_CharacterBase::GetMaxHealthAttribute(), NewMaxHp);
	ASC->SetNumericAttributeBase(UAS_CharacterBase::GetBaseAtkAttribute(), NewBaseAtk);

	ASC->SetNumericAttributeBase(
		UAS_CharacterBase::GetHealthAttribute(),
		NewMaxHp
	);
}