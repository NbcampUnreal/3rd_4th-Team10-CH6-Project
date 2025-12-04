#include "Structure/BuildSystem/BuildSystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "TTTGamePlayTags.h" 
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Structure/Crossbow/CrossbowStructure.h"

UBuildSystemComponent::UBuildSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 컴포넌트에서 RPC를 쓰기 위한 복제 설정
	SetIsReplicatedByDefault(true);
}

void UBuildSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}


// Called every frame
void UBuildSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 매 프레임 트레이스 실행
	TickBuildModeTrace();
}

void UBuildSystemComponent::ToggleBuildMode()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	APlayerController* PC = Cast<APlayerController>(Cast<APawn>(Owner)->GetController());
	if (!PC) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem || !IMC_Build) return;

	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) return;

	bool bIsBuildMode = ASC->HasMatchingGameplayTag(GASTAG::State_BuildMode);

	if (bIsBuildMode)
	{
		// [OFF]
		ASC->RemoveLooseGameplayTag(GASTAG::State_BuildMode);
		Subsystem->RemoveMappingContext(IMC_Build);

		if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
		{
			FGameplayEventData Payload;
			Payload.Instigator = Owner;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, GASTAG::Event_Cancel, Payload);
		}
		UE_LOG(LogTemp, Log, TEXT("[BuildSystem] Mode OFF"));
	}
	else
	{
		// [ON]
		ASC->AddLooseGameplayTag(GASTAG::State_BuildMode);
		Subsystem->AddMappingContext(IMC_Build, 10); // Priority 10
		UE_LOG(LogTemp, Log, TEXT("[BuildSystem] Mode ON"));
	}
}

void UBuildSystemComponent::SelectStructure(int32 SlotIndex)
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC || !ASC->HasMatchingGameplayTag(GASTAG::State_BuildMode)) return;

	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), GASTAG::Event_Cancel, Payload);
	}

	FGameplayTag TriggerTag;
	switch(SlotIndex)
	{
	case 1: TriggerTag = GASTAG::Event_Build_SelectStructure_1; break;
	case 2: TriggerTag = GASTAG::Event_Build_SelectStructure_2; break;
	case 3: TriggerTag = GASTAG::Event_Build_SelectStructure_3; break;
	case 4: TriggerTag = GASTAG::Event_Build_SelectStructure_4; break;
		// 더 추가 가능
	default: return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = GetOwner();
	Payload.EventMagnitude = SlotIndex; 

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), TriggerTag, Payload);
}

void UBuildSystemComponent::HandleConfirmAction()
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) return;

	// 1. 프리뷰 확정
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetOwner();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), GASTAG::Event_Confirm, Payload);
		return;
	}

	// 2. 업그레이드
	if (ASC->HasMatchingGameplayTag(GASTAG::State_BuildMode) && HoveredStructure)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuildComp] Upgrade Request -> %s"), *HoveredStructure->GetName());
		Server_InteractStructure(HoveredStructure, GASTAG::Event_Build_Upgrade);
	}
}

void UBuildSystemComponent::HandleCancelAction()
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) return;

	// 1. 프리뷰 취소
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetOwner();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), GASTAG::Event_Cancel, Payload);
		return;
	}

	// 2. 판매
	if (ASC->HasMatchingGameplayTag(GASTAG::State_BuildMode) && HoveredStructure)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuildComp] Sell Request -> %s"), *HoveredStructure->GetName());
		Server_InteractStructure(HoveredStructure, GASTAG::Event_Build_Sell);
	}
}

void UBuildSystemComponent::TickBuildModeTrace()
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC || !ASC->HasMatchingGameplayTag(GASTAG::State_BuildMode))
	{
		HoveredStructure = nullptr;
		return;
	}

	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		HoveredStructure = nullptr;
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
	if (!PC) return;

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	FVector TraceStart = CamLoc;
	FVector TraceEnd = TraceStart + (CamRot.Vector() * 1500.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

	// 디버그 라인
	if (bHit)
	{
		DrawDebugLine(GetWorld(), TraceStart, Hit.Location, FColor::Green, false, 0.f, 0, 1.f);
		DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Green, false, 0.f);
	}
	else
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 0.f, 0, 1.f);
	}

	AActor* HitActor = Hit.GetActor();
	if (bHit && HitActor && HitActor->IsA(ACrossbowStructure::StaticClass()))
	{
		if (HoveredStructure != HitActor)
		{
			HoveredStructure = HitActor;
			if(GEngine) GEngine->AddOnScreenDebugMessage(200, 1.f, FColor::Green, FString::Printf(TEXT("TARGET: %s"), *HitActor->GetName()));
		}
	}
	else
	{
		if (HoveredStructure != nullptr)
		{
			HoveredStructure = nullptr;
			if(GEngine) GEngine->AddOnScreenDebugMessage(200, 1.f, FColor::Red, TEXT("TARGET: NONE"));
		}
	}
}

void UBuildSystemComponent::Server_InteractStructure_Implementation(AActor* TargetActor, FGameplayTag InteractionTag)
{
	// 서버 로직
	if (!TargetActor) return;
	
	FGameplayEventData Payload;
	Payload.Instigator = GetOwner();
	Payload.Target = TargetActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), InteractionTag, Payload);

	UE_LOG(LogTemp, Warning, TEXT("[BuildComp Server] Event Sent: %s"), *InteractionTag.ToString());
}

UAbilitySystemComponent* UBuildSystemComponent::GetOwnerASC() const
{
	return CachedASC;
}

