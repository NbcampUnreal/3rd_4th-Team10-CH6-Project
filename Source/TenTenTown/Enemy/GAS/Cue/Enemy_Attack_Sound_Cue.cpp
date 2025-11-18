// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_Attack_Sound_Cue.h"

#include "TTTGamePlayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

bool UEnemy_Attack_Sound_Cue::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	Super::OnExecute_Implementation(Target, Parameters);
	
	const FGameplayTagContainer& SourceTags = Parameters.AggregatedSourceTags;
    
	TArray<FGameplayTag> MapKeys;
	SoundMap.GetKeys(MapKeys);
    
	const FGameplayTag* FoundTagPtr = MapKeys.FindByPredicate(
	[&SourceTags](const FGameplayTag& Tag)
	{
		return SourceTags.HasTagExact(Tag);	
	});
    
	if (FoundTagPtr)
	{
		if (USoundCue* const* SoundToPlayPtr = SoundMap.Find(*FoundTagPtr))
		{
			if (*SoundToPlayPtr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, *SoundToPlayPtr, Target->GetActorLocation());
				return true;
			}
		}
	}
	return false;
}

