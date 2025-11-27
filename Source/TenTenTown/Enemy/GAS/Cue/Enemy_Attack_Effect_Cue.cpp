// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GAS/Cue/Enemy_Attack_Effect_Cue.h"

#include "NiagaraFunctionLibrary.h"

bool UEnemy_Attack_Effect_Cue::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	Super::OnExecute_Implementation(Target, Parameters);

	const FGameplayTagContainer& SourceTags = Parameters.AggregatedSourceTags;
    
	TArray<FGameplayTag> MapKeys;
	EffectMap.GetKeys(MapKeys);
    
	const FGameplayTag* FoundTagPtr = MapKeys.FindByPredicate(
	[&SourceTags](const FGameplayTag& Tag)
	{
		return SourceTags.HasTagExact(Tag);	
	});
    
	if (FoundTagPtr)
	{
		if (UNiagaraSystem* const* EffectToPlayPtr = EffectMap.Find(*FoundTagPtr))
		{
			if (*EffectToPlayPtr)
			{
				float RandX = FMath::RandRange(-Offset, Offset);
				float RandY = FMath::RandRange(-Offset, Offset);
				float RandZ = FMath::RandRange(-Offset, Offset);
             
				FVector RandomOffsetVector(RandX, RandY, RandZ);

				FVector SpawnLocation = Parameters.Location + RandomOffsetVector;
             
				FRotator SpawnRotation = (Target) ? Target->GetActorRotation() : FRotator::ZeroRotator;
				
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				 Target,                               
				 *EffectToPlayPtr,                     
				 SpawnLocation,           
				 SpawnRotation,           
				 FVector(1.0f),                        
				 true,                                 
				 true                                  
			 );
				return true;
			}
		}
	}
	return false;
}
