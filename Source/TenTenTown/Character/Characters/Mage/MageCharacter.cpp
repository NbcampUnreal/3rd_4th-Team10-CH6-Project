//MageCharacter.cpp

#include "MageCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ENumInputID.h"
#include "Character/GAS/AS/MageAttributeSet/AS_MageAttributeSet.h"
#include "Character/GAS/GA/Mage/ComboAttack/GA_Mage_ComboAttack.h"
#include "Engine/LocalPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Character/InteractionSystemComponent/InteractionSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

AMageCharacter::AMageCharacter()
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
	CameraComponent->bUsePawnControlRotation=false;
	CameraComponent->SetupAttachment(SpringArmComponent,USpringArmComponent::SocketName);

	WandMesh=CreateDefaultSubobject<UStaticMeshComponent>("WandMesh");
	WandMesh->SetupAttachment(GetMesh(), WandAttachSocket);
	WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WandMesh->SetGenerateOverlapEvents(false);
	
	//점프 횟수
	JumpMaxCount=2;

	PrimaryActorTick.bCanEverTick = true;
	ISC = CreateDefaultSubobject<UInteractionSystemComponent>("ISC");
}

void AMageCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	WandMesh->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			WandAttachSocket);
	WandMesh->SetSimulatePhysics(false);
	WandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WandMesh->SetGenerateOverlapEvents(false);
}

UStaticMeshComponent* AMageCharacter::FindStaticMeshCompByName(FName Name) const
{
	TArray<UStaticMeshComponent*> SMs;
	const_cast<AMageCharacter*>(this)->GetComponents<UStaticMeshComponent>(SMs);
	for (UStaticMeshComponent* C : SMs)
	{
		if (C && C->GetName() == Name)
		{
			return C;
		}
	}
	return nullptr;
}

void AMageCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	
	PS = Cast<ATTTPlayerState>(GetPlayerState());
	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		MageAS = Cast<UAS_MageAttributeSet>(ASC->GetAttributeSet(UAS_MageAttributeSet::StaticClass()));
	}

	for (const auto& IDnAbility : InputIDGAMap)
	{
		const auto& [InputID, Ability] = IDnAbility;
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

void AMageCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PS = Cast<ATTTPlayerState>(GetPlayerState());
	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		MageAS = Cast<UAS_MageAttributeSet>(ASC->GetAttributeSet(UAS_MageAttributeSet::StaticClass()));
	}
	
	ASC->InitAbilityActorInfo(PS,this);
}

void AMageCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!MageAS) return;
	
	const float H  = MageAS->GetHealth();
	const float MH = MageAS->GetMaxHealth();
	const float M  = MageAS->GetMana();
	const float MM = MageAS->GetMaxMana();
	const float L  = MageAS->GetLevel();
	
	const FString Msg = FString::Printf(TEXT("HP %.0f/%.0f | MANA %0.f/%0.f | LV %.0f"), H, MH, M, MM, L);
	
	if (GEngine) GEngine->AddOnScreenDebugMessage(1001, 0.f, FColor::Cyan, Msg);
}

void AMageCharacter::GiveDefaultAbility()
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

void AMageCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Jump);
		EIC->BindAction(BlinkAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Dash);
		EIC->BindAction(FireballAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillA);
		EIC->BindAction(FlameWallAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillB);
		EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::NormalAttack);
		EIC->BindAction(FlameThrowerAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
		EIC->BindAction(FlameThrowerAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
		EIC->BindAction(FlameThrowerAction, ETriggerEvent::Canceled, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
	}
}

void AMageCharacter::Move(const FInputActionInstance& FInputActionInstance)
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

void AMageCharacter::Look(const FInputActionInstance& FInputActionInstance)
{
	const FVector InputVector = FInputActionInstance.GetValue().Get<FVector>();
	AddControllerYawInput(InputVector.X);
	AddControllerPitchInput(InputVector.Y);
}

void AMageCharacter::ActivateGAByInputID(const FInputActionInstance& FInputActionInstance, ENumInputID InputID)
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