// Fill out your copyright notice in the Description page of Project Settings.


#include "TA_FighterSquare.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

ATA_FighterSquare::ATA_FighterSquare()
{
	bDestroyOnConfirmation=true;
	PrimaryActorTick.bCanEverTick=false;
	PrimaryActorTick.bStartWithTickEnabled=false;
}

void ATA_FighterSquare::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	Character=Cast<ACharacter>(Ability->GetAvatarActorFromActorInfo());
	ASC=Ability->GetAbilitySystemComponentFromActorInfo();
}

void ATA_FighterSquare::ConfirmTargetingAndContinue()
{
	//Super::ConfirmTargetingAndContinue();

	TArray<FOverlapResult> OutOverlaps;
	const FVector Pos = Character->GetActorLocation()+Character->GetActorForwardVector()*ShapePos;
	const FQuat Rot = Character->GetActorForwardVector().ToOrientationQuat();
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	FCollisionShape Shape= FCollisionShape::MakeBox(Extent);
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(NormalAttackOverlap),false,this);
	
	GetWorld()->OverlapMultiByObjectType(OutOverlaps,Pos,Rot,ObjectQueryParams,Shape,QueryParams);
	DrawDebugBox(GetWorld(),Pos,Extent,Rot, FColor::Green,false,2.f,0,2.f);

	FGameplayAbilityTargetData_ActorArray* TargetData_ActorArray = new FGameplayAbilityTargetData_ActorArray();

	for (const auto& OverlapResult : OutOverlaps)
	{
		AActor* Actor = OverlapResult.GetActor();
		if (Actor&&Actor!=Character)
		{
			TargetData_ActorArray->TargetActorArray.Add(Actor);
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData_ActorArray);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATA_FighterSquare::ConfirmTargeting()
{
	Super::ConfirmTargeting();
}

void ATA_FighterSquare::CancelTargeting()
{
	Super::CancelTargeting();
}