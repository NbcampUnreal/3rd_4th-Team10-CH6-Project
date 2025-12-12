// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_Guard_Effect_Cue.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void AEnemy_Guard_Effect_Cue::HandleGameplayCue(
	AActor* Target,
	EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters
)
{
	Super::HandleGameplayCue(Target, EventType, Parameters);

	if (!Target)
	{
		return;
	}
	ACharacter* Character = Cast<ACharacter>(Target);
	USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;

	if (EventType == EGameplayCueEvent::Executed)
	{
		if (Mesh && GuardEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				GuardEffect,
				Mesh,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTargetIncludingScale,
				true
			);
		}
	}
}
