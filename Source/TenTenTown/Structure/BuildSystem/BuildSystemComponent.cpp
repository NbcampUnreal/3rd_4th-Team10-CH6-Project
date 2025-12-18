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
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "Structure/GridSystem/GridFloorActor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

UBuildSystemComponent::UBuildSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 컴포넌트에서 RPC를 쓰기 위한 복제 설정
	SetIsReplicatedByDefault(true);

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Script/Engine.SoundWave'/Game/Structure/StructureGC/SFX/SFX_Buildmode.SFX_Buildmode'"));
    
	if (SoundAsset.Succeeded())
	{
		BuildModeSound = SoundAsset.Object;
	}
}

void UBuildSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBuildSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
	// [OFF]
	if (bIsBuildMode)
	{
		ASC->RemoveLooseGameplayTag(GASTAG::State_BuildMode);
		Subsystem->RemoveMappingContext(IMC_Build);

		if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
		{
			FGameplayEventData Payload;
			Payload.Instigator = Owner;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, GASTAG::Event_Cancel, Payload);
		}

		if (BuildModeSound && PC)
		{
			if (PC->IsLocalController())
			{
				UGameplayStatics::PlaySound2D(this, BuildModeSound);
			}
		}
		
		UE_LOG(LogTemp, Log, TEXT("[BuildSystem] Mode OFF"));
	}
	// [ON]
	else
	{
		// 빌드모드 Build 또는 Combat 페이즈가 아니면...
		// 필요 시 주석 해제
		/*UWorld* World = GetWorld();
		if (World)
		{
			ATTTGameStateBase* GameState = Cast<ATTTGameStateBase>(World->GetGameState());
			if (GameState)
			{
				// 현재 페이즈 가져오기
				ETTTGamePhase CurrentPhase = GameState->Phase;

				// Build 또는 Combat 페이즈가 아니면 -> 진입 불가
				bool bIsAllowed = (CurrentPhase == ETTTGamePhase::Build || CurrentPhase == ETTTGamePhase::Combat);
				
				if (!bIsAllowed)
				{
					UE_LOG(LogTemp, Warning, TEXT("[BuildSystem] 현재 페이즈(%d)에서는 빌드 모드를 켤 수 없습니다."), (int32)CurrentPhase);
					return; 
				}
			}
		}*/
		ASC->AddLooseGameplayTag(GASTAG::State_BuildMode);
		Subsystem->AddMappingContext(IMC_Build, 10); // IMC 우선도 설정(EIS)

		if (BuildModeSound && PC)
		{
			if (PC->IsLocalController())
			{
				UGameplayStatics::PlaySound2D(this, BuildModeSound);
			}
		}
		
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
	case 5: TriggerTag = GASTAG::Event_Build_SelectStructure_5; break;
	case 6: TriggerTag = GASTAG::Event_Build_SelectStructure_6; break;
	case 7: TriggerTag = GASTAG::Event_Build_SelectStructure_7; break;
	case 8: TriggerTag = GASTAG::Event_Build_SelectStructure_8; break;
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

	// 확정
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetOwner();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), GASTAG::Event_Confirm, Payload);
		return;
	}

	// 업그레이드
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

	// 취소
	if (ASC->HasMatchingGameplayTag(GASTAG::State_IsSelecting))
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetOwner();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), GASTAG::Event_Cancel, Payload);
		return;
	}

	// 판매
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
	if (bHit && HitActor && HitActor->IsA(AStructureBase::StaticClass()))
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
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

