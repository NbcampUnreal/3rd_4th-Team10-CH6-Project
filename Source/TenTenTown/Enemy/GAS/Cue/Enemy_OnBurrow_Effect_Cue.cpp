// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_OnBurrow_Effect_Cue.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

AEnemy_OnBurrow_Effect_Cue::AEnemy_OnBurrow_Effect_Cue()
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = DefaultSceneRoot;
	
	bAutoDestroyOnRemove = false;
}

void AEnemy_OnBurrow_Effect_Cue::HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type EventType,
                                                   const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(Target, EventType, Parameters);

	USkeletalMeshComponent* TargetMesh = Cast<ACharacter>(Target) ? Cast<ACharacter>(Target)->GetMesh() : nullptr;

	switch (EventType)
	{
	case EGameplayCueEvent::OnActive:
		{
			if (TargetMesh && BurrowEffect)
			{
				if (ActiveBurrowEffect) return;

				ActiveBurrowEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
					BurrowEffect, 
					TargetMesh, 
					NAME_None,
					FVector::ZeroVector, 
					FRotator::ZeroRotator, 
					EAttachLocation::KeepRelativeOffset, 
					true
				);
			}
			break;
		}

	case EGameplayCueEvent::Removed:
		{
			if (ActiveBurrowEffect)
			{
				ActiveBurrowEffect->Deactivate();
				ActiveBurrowEffect = nullptr;
			}
            
			Destroy(); 
			break;
		}

	//case EGameplayCueEvent::WhileActive:
	//	{
	//		break;
	//	}

	default:
		break;
	}
}
