// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcherFloatingPawn.h"

#include <rapidjson/document.h>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AArcherFloatingPawn::AArcherFloatingPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponents");
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldDynamic,ECollisionResponse::ECR_Block);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic,ECollisionResponse::ECR_Block);
	SetRootComponent(SphereComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponents");
	SpringArmComponent->TargetArmLength=1.f;
	SpringArmComponent->bUsePawnControlRotation=true;
	SpringArmComponent->SetupAttachment(SphereComponent);

	CameraComponent=CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent,USpringArmComponent::SocketName);
	
	FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");
	DecalComponent = CreateDefaultSubobject<UDecalComponent>("DecalComponent");
	DecalComponent->SetupAttachment(RootComponent);
	DecalComponent->DecalSize = FVector(256.f,1800.f,1800.f);
	DecalComponent->SetRelativeRotation(FRotator(-90.f,0.f,0.f));
	DecalComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void AArcherFloatingPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		CrosshairWidget = CreateWidget(PC,CrosshairClass);
		
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}
	}
	
	if (DecalComponent&&DecalMaterial)
	{
		DecalComponent->SetDecalMaterial(DecalMaterial);
	}
}


// Called to bind functionality to input
void AArcherFloatingPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			UEnhancedInputLocalPlayerSubsystem* EILPS = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			
			if (EILPS)
			{
				if (IMCGhost)
				{
					EILPS->AddMappingContext(IMCGhost,0);	
				}
			}
		}
	}
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (EIC)
	{
		EIC->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ThisClass::Move);
		EIC->BindAction(AscendAction,ETriggerEvent::Triggered,this,&ThisClass::Ascend);
	}
}

void AArcherFloatingPawn::Move(const FInputActionValue& Value)
{
	FVector InputVector= Value.Get<FVector>();
	
	if (Controller)
	{
		FRotator ControllerRotation = GetControlRotation();
		
		FVector ForwardVector = FRotationMatrix(ControllerRotation).GetUnitAxis(EAxis::X);
		FVector RightVector = FRotationMatrix(ControllerRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardVector,InputVector.X);
		AddMovementInput(RightVector,InputVector.Y);
	}
}

void AArcherFloatingPawn::Ascend(const FInputActionValue& Value)
{
	float Scalar = Value.Get<float>();
	
	if (Controller && Scalar != 0.f)
	{
		AddMovementInput(FVector::UpVector, Scalar);
	}
}

void AArcherFloatingPawn::ConfirmLocation(const FInputActionValue& Value)
{
}

