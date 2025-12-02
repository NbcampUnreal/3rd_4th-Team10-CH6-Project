// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CharacterRevive.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Characters/Base/BaseCharacter.h"

UGA_CharacterRevive::UGA_CharacterRevive()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag= GASTAG::Event_Character_Revive;
	TriggerData.TriggerSource=EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UGA_CharacterRevive::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Green,FString::Printf(TEXT("Hello this is revive activate")));
	
	ASC = GetAbilitySystemComponentFromActorInfo();
	AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	ReviveMontage = Cast<ABaseCharacter>(AvatarCharacter)->GetReviveMontage();
	LastMovementMode =static_cast<EMovementMode>(TriggerEventData->EventMagnitude);
	
	TArray<AActor*> PlayerStartLocation;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(),PlayerStartLocation);
	
	if (HasAuthority(&ActivationInfo)&&PlayerStartLocation.Num()>0)
	{
		int32 RandomInt = FMath::RandRange(0,PlayerStartLocation.Num()-1);
		FVector RandomSpawnPos = PlayerStartLocation[RandomInt]->GetActorLocation();
		AvatarCharacter->SetActorLocation(RandomSpawnPos);
	}
	
	UAbilityTask_PlayMontageAndWait* ReviveAMTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,FName("ReviveAnim"),ReviveMontage);
	ReviveAMTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageEnd);
	ReviveAMTask->ReadyForActivation();
}

void UGA_CharacterRevive::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CharacterRevive::OnMontageEnd()
{
	AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	AvatarCharacter->bUseControllerRotationYaw=true;
	ASC->RemoveLooseGameplayTag(GASTAG::State_Character_Dead);
	
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
