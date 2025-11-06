// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Fireball.h"

#include "DrawDebugHelpers.h"
#include "Fireball_Projectile.h"
#include "LocalizationConfigurationScript.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimSequence.h"
#include "Character/PS/TTTPlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Fireball::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASC = Cast<ATTTPlayerState>(GetOwningActorFromActorInfo())->GetAbilitySystemComponent();
	AvatarCharacter=Cast<ACharacter>(GetAvatarActorFromActorInfo());
	OriginSpeed = AvatarCharacter->GetCharacterMovement()->GetMaxSpeed();
	ChargingSeconds=0.f;
	
	
	if (!ASC||!AvatarCharacter)
	{
		UE_LOG(LogTemp,Log,TEXT("no asc or no avatar character"));
		return;
	}
	
	auto* AMTaskFireball = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("AMTaskFireball"),FireballMontage,1.f,"Start");
	
	
	auto* WaitGameplayEventTask_Charging = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fireball_Charging,nullptr,true);
	WaitGameplayEventTask_Charging->EventReceived.AddUniqueDynamic(this,&ThisClass::ActiveLoopGameplayCue);

	auto* WaitGameplayEventTask_Release = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,GASTAG::Event_Fireball_Release,nullptr,true);
	WaitGameplayEventTask_Release->EventReceived.AddUniqueDynamic(this,&ThisClass::LaunchFireball);
	
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=100.f;
	GetWorld()->GetTimerManager().SetTimer(ChargingSecondHandle,
		[&]()
		{ChargingSeconds+=0.05f;},
		0.05f,
		true,
		-1
		);
	WaitGameplayEventTask_Charging->ReadyForActivation();
	WaitGameplayEventTask_Release->ReadyForActivation();
	AMTaskFireball->ReadyForActivation();
	
	ASC->ForceReplication();
}

void UGA_Fireball::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Fireball::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	GetWorld()->GetTimerManager().ClearTimer(ChargingSecondHandle);

	auto* AMTaskRelease = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("AMTaskRelease"),FireballReleaseMontage,1.5f);
	AMTaskRelease->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnAbilityEnd);
	AMTaskRelease->ReadyForActivation();
	ASC->RemoveGameplayCue(GASTAG::GameplayCue_Fireball_Charging);
	UE_LOG(LogTemp,Log,TEXT("%f charge secs"),ChargingSeconds);
	ASC->ForceReplication();
}

void UGA_Fireball::OnAbilityEnd()
{
	AvatarCharacter->GetCharacterMovement()->MaxWalkSpeed=OriginSpeed;
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_Fireball::ActiveLoopGameplayCue(const FGameplayEventData Data)
{
	ASC->AddGameplayCue(GASTAG::GameplayCue_Fireball_Charging);
}

void UGA_Fireball::LaunchFireball(const FGameplayEventData Data)
{
	APlayerController* PC = Cast<APlayerController>(AvatarCharacter->GetController());
	FVector Start;
	FRotator Rotation;
	PC->GetPlayerViewPoint(Start,Rotation);

	FVector End = Rotation.Vector()*10000.f+Start;
	
	FHitResult HitResult;
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(AvatarCharacter);
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	
	bool bSuccessLineTrace = GetWorld()->LineTraceSingleByObjectType(HitResult,Start,End,ObjectQueryParams,CollisionQueryParams);
	DrawDebugLine(GetWorld(),Start,End,FColor::Green,true);

	FTransform SpawnTransform;
	FVector SpawnLocation = AvatarCharacter->GetActorLocation()+AvatarCharacter->GetActorForwardVector()*100.f;
	SpawnLocation.Z=AvatarCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FRotator SpawnRotator = AvatarCharacter->GetActorRotation();
	
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotator.Quaternion());
	
	if (bSuccessLineTrace)
	{
		UE_LOG(LogTemp,Log,TEXT("%f %f"),Start.X,End.X);

		AFireball_Projectile* Proj = GetWorld()->SpawnActorDeferred<AFireball_Projectile>(Projectile
			,SpawnTransform,AvatarCharacter,AvatarCharacter,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			,ESpawnActorScaleMethod::MultiplyWithRoot);

		if (!Projectile) UE_LOG(LogTemp,Log,TEXT("no projectile spawn"));
		Proj->FinishSpawning(SpawnTransform);
		FVector Direction = HitResult.Location-AvatarCharacter->GetActorLocation();
		Proj->FireProjectile(Direction,AvatarCharacter);
	}
	else
	{
		UE_LOG(LogTemp,Log,TEXT("%f %f"),Start.X,End.X);

		AFireball_Projectile* Proj = GetWorld()->SpawnActorDeferred<AFireball_Projectile>(Projectile
			,SpawnTransform,AvatarCharacter,AvatarCharacter,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			,ESpawnActorScaleMethod::MultiplyWithRoot);
		if (!Projectile) UE_LOG(LogTemp,Log,TEXT("no projectile spawn"));
		
		Proj->FinishSpawning(SpawnTransform);
		
		Proj->FireProjectile(End-Start,AvatarCharacter);
	}
}

