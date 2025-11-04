#include "GA_Blink.h"

#include "AbilitySystemComponent.h"
#include "GE_BlinkCooldown.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UGA_Blink::UGA_Blink()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Blink")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Blink")));
	
	CooldownGameplayEffectClass = UGE_BlinkCooldown::StaticClass();
	BlinkCooldownTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Blink")));
}

const FGameplayTagContainer* UGA_Blink::GetCooldownTags() const
{
	return &BlinkCooldownTags;
}

void UGA_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FVector Dest;
	if (!bFindBlinkDest(Character, Dest))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (ActorInfo->IsNetAuthority())
	{
		Character->TeleportTo(Dest, Character->GetActorRotation(), false, true);
	}
	else
	{
		//연출 효과 넣기 (GC)
	}

	if (auto* Wait = UAbilityTask_WaitDelay::WaitDelay(this, 0.35f))
	{
		Wait->OnFinish.AddDynamic(this, &UGA_Blink::OnBlinkDelayFinished);
		Wait->ReadyForActivation();
		return;
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Blink::ApplyCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (!CooldownGameplayEffectClass) return;

	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass, GetAbilityLevel());
	if (!Spec.IsValid()) return;

	static const FGameplayTag TAG_Data_Cooldown = FGameplayTag::RequestGameplayTag(TEXT("Data.Cooldown"));
	Spec.Data->SetSetByCallerMagnitude(TAG_Data_Cooldown, CooldownSec);

	if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
	{
		FScopedPredictionWindow Prediction(ASC, true);
		if (ActorInfo->IsNetAuthority())
		{
			(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
		}
	}
}

bool UGA_Blink::bFindBlinkDest(const ACharacter* Character, FVector& OutDest) const
{
	const FVector Start = Character->GetActorLocation();

	//캐릭터가 이동하는 방향으로 점멸
	FVector Dir = Character->GetCharacterMovement()->GetLastInputVector();
	
	//이동이 없다면 정면으로 점멸
	if (Dir.IsNearlyZero())
	{
		Dir = Character->GetActorForwardVector();
	}
	Dir.Normalize();
	
	const FVector End = Start + Dir * MaxDist;

	//벽을 감지하여 벽보다 조금 앞으로 점멸
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Blink), false, Character);
	const bool bHit = Character->GetWorld()->SweepSingleByChannel(
		Hit,
		Start, End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		Params);

	//벽이 없다면 최대거리 점멸
	if (!bHit)
	{
		OutDest = End;
		return true;
	}

	OutDest = Hit.Location - Dir * (TraceRadius * 5.f);
	
	//도착지점 바닥을 감지하여 땅으로 점멸
	FHitResult Ground;
	const FVector Up = OutDest + FVector(0, 0, 200);
	const FVector Down = OutDest - FVector(0, 0, 1000);
	const bool bGround = Character->GetWorld()->LineTraceSingleByChannel(
		Ground,
		Up, Down,
		ECC_Visibility,
		Params);

	if (bGround)
	{
		const float HalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		OutDest = Ground.Location + FVector(0, 0, HalfHeight);
	}
	
	return true;
}

void UGA_Blink::OnBlinkDelayFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}