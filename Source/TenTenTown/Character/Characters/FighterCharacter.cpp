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

// Sets default values
AFighterCharacter::AFighterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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
	CameraComponent->bUsePawnControlRotation=false;
	CameraComponent->SetupAttachment(SpringArmComponent,USpringArmComponent::SocketName);
	
	//점프 횟수
	JumpMaxCount=2;
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
	}

	for (const auto& IDnAbility : InputIDGAMap)
	{
		const auto& [InputID,Ability]=IDnAbility;
		FGameplayAbilitySpec Spec(Ability,1,static_cast<int32>(InputID));
		ASC->GiveAbility(Spec);
	}

	ASC->InitAbilityActorInfo(PS,this);
}

//클라이언트에서 실행됨
void AFighterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PS = Cast<ATTTPlayerState>(GetPlayerState());
	ASC = PS->GetAbilitySystemComponent();

	ASC->InitAbilityActorInfo(PS,this);
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
			Spec->InputPressed=true;
			if (Spec->IsActive()) ASC->AbilitySpecInputPressed(*Spec);
			else ASC->TryActivateAbility(Spec->Handle);
			break;
		case ETriggerEvent::Canceled:
		case ETriggerEvent::Completed:
			Spec->InputPressed=false;
			if (Spec->IsActive()) ASC->AbilitySpecInputReleased(*Spec);
			break;
		case ETriggerEvent::Ongoing:
			break;
		case ETriggerEvent::None:
			break;
		}
	}
}
