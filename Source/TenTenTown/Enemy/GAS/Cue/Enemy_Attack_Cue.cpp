// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_Attack_Cue.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

bool UEnemy_Attack_Cue::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	Super::OnExecute_Implementation(Target, Parameters);

	if (!Target) return false;

	USoundCue* SoundToPlay = nullptr;
	const FGameplayTagContainer& Tags = Parameters.AggregatedSourceTags;
 
	if (Tags.HasTag(GASTAG::Enemy_Type_Worm))
	{
		SoundToPlay = WormSoundCue;
	}
	else if (Tags.HasTag(GASTAG::Enemy_Type_Orc))
	{
		SoundToPlay = OrcSoundCue;
	}
	else if (Tags.HasTag(GASTAG::Enemy_Type_Dragon))
	{
		SoundToPlay = DragonSoundCue;
	}
	else if (Tags.HasTag(GASTAG::Enemy_Type_Beholder))
	{
		SoundToPlay = BeholderSoundCue;
	}

	if (SoundToPlay)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, Target->GetActorLocation());
		return true;
	}
	return false;
}

