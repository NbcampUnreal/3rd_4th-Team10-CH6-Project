// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Camera/CameraComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ENumInputID.h"
#include "AttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Character/InteractionSystemComponent/InteractionSystemComponent.h"
#include "Engine/LocalPlayer.h"

// Sets default values
AFighterCharacter::AFighterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	
	bUseControllerRotationYaw=true;
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;

	GetCapsuleComponent()->SetCapsuleHalfHeight(70.f);
	
	GetCharacterMovement()->bOrientRotationToMovement=false;
	GetCharacterMovement()->MaxWalkSpeed=300.f;

	SpringArmComponent= CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->bUsePawnControlRotation=true;
	SpringArmComponent->TargetArmLength=300.f;
	SpringArmComponent->SocketOffset = FVector(0.0f,115.f,65.f);
	SpringArmComponent->bDoCollisionTest=true;
	SpringArmComponent->ProbeSize = 10.f;
	SpringArmComponent->ProbeChannel =ECC_Camera;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->bUsePawnControlRotation=false;
	CameraComponent->SetupAttachment(SpringArmComponent,USpringArmComponent::SocketName);
	
	//점프 횟수
	JumpMaxCount=2;
	ISC=CreateDefaultSubobject<UInteractionSystemComponent>("ISC");
}

// Called when the game starts or when spawned
void AFighterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

//서버에서만 실행됨
void AFighterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PS = Cast<ATTTPlayerState>(GetPlayerState());
	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		FighterAttributeSet = Cast<UAS_FighterAttributeSet>(ASC->GetAttributeSet(UAS_FighterAttributeSet::StaticClass()));
	}

	for (const auto& IDnAbility : InputIDGAMap)
	{
		const auto& [InputID,Ability]=IDnAbility;
		FGameplayAbilitySpec Spec(Ability,1,static_cast<int32>(InputID));
		ASC->GiveAbility(Spec);
	}

	for (const auto& Attribute : AttributeSets)
	{
		UAttributeSet* AttributeSet = NewObject<UAttributeSet>(PS,Attribute);
		ASC->AddAttributeSetSubobject(AttributeSet);
	}

	ASC->InitAbilityActorInfo(PS,this);
}

//클라이언트에서 실행됨
void AFighterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	PS=Cast<ATTTPlayerState>(GetPlayerState());
	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		FighterAttributeSet = Cast<UAS_FighterAttributeSet>(ASC->GetAttributeSet(UAS_FighterAttributeSet::StaticClass()));
	}
	ASC->InitAbilityActorInfo(PS,this);
}

// Called every frame
void AFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FighterAttributeSet)
	{
		return;
	}
	
	const float H  = FighterAttributeSet->GetHealth();
	const float MH = FighterAttributeSet->GetMaxHealth();
	const float S  = FighterAttributeSet->GetStamina();
	const float MS = FighterAttributeSet->GetMaxStamina();
	const float L  = FighterAttributeSet->GetLevel();

	const FString Msg = FString::Printf(TEXT("HP %.0f/%.0f | STAMINA %.0f/%.0f | LV %.0f"), H, MH, S, MS, L);
	
	if (GEngine) GEngine->AddOnScreenDebugMessage(1001, 0.f, FColor::Cyan, Msg);
}

// Called to bind functionality to input
void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EIC->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ThisClass::Move);
		EIC->BindAction(LookAction,ETriggerEvent::Triggered,this,&ThisClass::Look);
		EIC->BindAction(JumpAction,ETriggerEvent::Started,this,&ThisClass::ActivateGAByInputID,ENumInputID::Jump);
		EIC->BindAction(DashAction,ETriggerEvent::Started,this,&ThisClass::ActivateGAByInputID,ENumInputID::Dash);
		EIC->BindAction(RightChargeAttack,ETriggerEvent::Triggered,this,&ThisClass::ActivateGAByInputID,ENumInputID::RightChargeAttack);
		EIC->BindAction(RightChargeAttack,ETriggerEvent::Completed,this,&ThisClass::ActivateGAByInputID,ENumInputID::RightChargeAttack);
	}
}

void AFighterCharacter::Move(const FInputActionInstance& FInputActionInstance)
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

void AFighterCharacter::Look(const FInputActionInstance& FInputActionInstance)
{
	const FVector InputVector = FInputActionInstance.GetValue().Get<FVector>();
	AddControllerYawInput(InputVector.X);
	AddControllerPitchInput(InputVector.Y);
}

void AFighterCharacter::ActivateGAByInputID(const FInputActionInstance& FInputActionInstance, ENumInputID InputID)
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
