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
		return SourceTags.HasTag(Tag);	
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
				 Target,                               // 월드를 가져오기 위해 Target 액터 제공
				 *EffectToPlayPtr,                     // 재생할 UNiagaraSystem 객체
				 SpawnLocation,           // 이펙트가 스폰될 위치
				 SpawnRotation,           // 이펙트의 회전
				 FVector(1.0f),                        // 스케일 (기본값)
				 true,                                 // AutoDestroy: 완료되면 자동 파괴
				 true                                  // Required
			 );
				return true;
			}
		}
	}
	return false;
}
