#include "GA_Mage_FlameThrower.h"

#include "FlameThrowerActor.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Characters/Mage/MageCharacter.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Mage_FlameThrower::UGA_Mage_FlameThrower()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Mage.FlameThrower")));

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
}

void UGA_Mage_FlameThrower::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bInputHeld = true;

	ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Char)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle,  CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	auto Lock = [this](ACharacter* C)
	{
		if (auto* Move = C->GetCharacterMovement())
		{
			SavedMoveMode = Move->MovementMode;
			Move->StopMovementImmediately();
			Move->DisableMovement();
		}
		if (APlayerController* PC = Cast<APlayerController>(C->GetController()))
		{
			PC->SetIgnoreMoveInput(true);
		}
	};
	
	if (CurrentActorInfo->IsNetAuthority())
		Lock(Char);
	if (Char->IsLocallyControlled())
		Lock(Char);

	if (ChargeMontage)
	{
		ChargeTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChargeMontage, 1,NAME_None, false);
		ChargeTask->OnCompleted.AddDynamic(this, &ThisClass::OnChargeComplete);
		ChargeTask->ReadyForActivation();
	}
		
	if (ActorInfo->IsLocallyControlled())
	{
		Char->GetWorldTimerManager().SetTimer(
		ChargeTimer,
		this,
		&UGA_Mage_FlameThrower::OnChargeComplete,
		ChargeHoldTime,
		false
	);
	}
}

void UGA_Mage_FlameThrower::OnChargeComplete()
{
	if (!bInputHeld)
	{
		EndAbility(CurrentSpecHandle,  CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AMageCharacter* Mage = Cast<AMageCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Mage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (UAnimInstance* Anim = Mage->GetMesh() ? Mage->GetMesh()->GetAnimInstance() : nullptr)
	{
		if (ChargeMontage)
		{
			Anim->Montage_Stop(0.2f, ChargeMontage);
		}
	}

	FGameplayTag::RequestGameplayTag(TEXT("State.Channeling.FlameThrower"));

	OnShootEvent();

	if (ChannelMontage)
	{
		ChannelTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChannelMontage, 1,NAME_None, false);
		ChannelTask->ReadyForActivation();
	}
	
	Mage->GetWorldTimerManager().SetTimer(
		ChannelTimer,
		[this](){ EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); },
		MaxChannelTime,
		false
	);
}

void UGA_Mage_FlameThrower::OnShootEvent()
{
	if (!CurrentActorInfo->IsNetAuthority())
	{
		ServerSpawnFlame();
	}
}

void UGA_Mage_FlameThrower::ServerSpawnFlame_Implementation()
{
	SpawnFlame();
}
 
void UGA_Mage_FlameThrower::SpawnFlame()
{
	ACharacter* Char = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Char || !Char->HasAuthority() || !FlameClass) return;
	if (SpawnedActor) return;

	FVector EyeLoc;
	FRotator EyeRot;
	Char->GetActorEyesViewPoint(EyeLoc, EyeRot);
	
	const FVector Start = EyeLoc + EyeRot.Vector() * 30.f;
	const FRotator Rot = EyeRot;
	
	FActorSpawnParameters P;
	P.Owner = Char;
	P.Instigator = Char;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AFlameThrowerActor* Flame = Char->GetWorld()->SpawnActor<AFlameThrowerActor>(FlameClass, Start, Rot, P))
	{
		Flame->SetOwner(Char);
		Flame->SetReplicateMovement(true);
		Flame->SetNetUpdateFrequency(120.f);
		Flame->SetMinNetUpdateFrequency(60.f);
		Flame->SetNetDormancy(DORM_Awake);
		
		Flame->Init(TraceInterval, ConeHalfAngleDeg, MaxChannelTime);

		Char->MoveIgnoreActorAdd(Flame);
		SpawnedActor = Flame;
	}
}

void UGA_Mage_FlameThrower::OnChannelMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Mage_FlameThrower::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	bInputHeld = false;
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UGA_Mage_FlameThrower::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		auto& TM = ActorInfo->AvatarActor->GetWorldTimerManager();
		TM.ClearTimer(ChargeTimer);
		TM.ClearTimer(ChannelTimer);
	}

	if (ActorInfo && ActorInfo->IsNetAuthority() && SpawnedActor)
	{
		SpawnedActor->Destroy();
		SpawnedActor = nullptr;
	}

	if (ChargeTask)
	{
		ChargeTask->EndTask();
		ChargeTask = nullptr;
	}
	if (ChannelTask)
	{
		ChannelTask->EndTask();
		ChannelTask = nullptr;
	}
	
	if (ActorInfo)
	{
		if (ACharacter* C = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (UAnimInstance* Anim = C->GetMesh() ? C->GetMesh()->GetAnimInstance() : nullptr)
			{
				if (ChargeMontage) Anim->Montage_Stop(0.2f, ChargeMontage);
				if (ChannelMontage) Anim->Montage_Stop(0.2f, ChannelMontage);
			}
			auto Unlock = [this](ACharacter* X)
			{
				if (auto* Move = X->GetCharacterMovement())
				{
					const bool bSavedValid = (SavedMoveMode != MOVE_None && SavedMoveMode != MOVE_Custom);
					Move->SetMovementMode(bSavedValid ? SavedMoveMode : MOVE_Walking);
				}
				if (APlayerController* PC = Cast<APlayerController>(X->GetController()))
				{
					PC->SetIgnoreMoveInput(false);
				}
			};
			
			if (ActorInfo->IsNetAuthority())
				Unlock(C);
			
			if (C->IsLocallyControlled())
				Unlock(C);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
