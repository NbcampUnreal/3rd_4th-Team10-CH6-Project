// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorArrowRain.h"

#include "TimerManager.h"
#include "Character/Characters/Archer/Arrow/Archer_Arrow.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/System/SpawnSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AActorArrowRain::AActorArrowRain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);
	
}

void AActorArrowRain::SetArrowClass(TSubclassOf<AArcher_Arrow> NewArrowClass)
{
	ArrowClass=NewArrowClass;
}

void AActorArrowRain::InitializeArrowRain(float InDamage, TSubclassOf<UGameplayEffect> InGEClass)
{
	ArrowDamage = InDamage;
	DamageGEClass = InGEClass;
}

// Called when the game starts or when spawned
void AActorArrowRain::BeginPlay()
{
	Super::BeginPlay();
	
	if (ArrowClass)
	{
		GetWorldTimerManager().SetTimer(RainTimerHandle, this, &AActorArrowRain::SpawnSingleArrow, SpawnInterval, true);
	}
}

// Called every frame
void AActorArrowRain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorArrowRain::SpawnSingleArrow()
{
	if (ArrowsSpawnedSoFar >= TotalArrowCount || !ArrowClass)
	{
		GetWorldTimerManager().ClearTimer(RainTimerHandle);
		SetLifeSpan(3.0f);
		return;
	}

	FVector RandomOffset = UKismetMathLibrary::RandomPointInBoundingBox(FVector::ZeroVector, FVector(RainRadius, RainRadius, 0));
	FVector SpawnLocation = GetActorLocation() + RandomOffset + FVector(0, 0, SpawnHeight);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner(); 
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AArcher_Arrow* NewArrow = GetWorld()->SpawnActor<AArcher_Arrow>(ArrowClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (NewArrow)
	{
		NewArrow->SetDamage(ArrowDamage);
		if (DamageGEClass)
		{
			NewArrow->SetSetByCallerClass(DamageGEClass);
		}

		
		FVector BaseDirection = FVector::DownVector; 
		
		float ConeHalfAngleRad = FMath::DegreesToRadians(MaxTiltAngle);
		
		FVector FallDirection = FMath::VRandCone(BaseDirection, ConeHalfAngleRad);

		NewArrow->FireArrow(FallDirection, 1.0f);

		NewArrow->Multicast_PlayEffects();
		ArrowsSpawnedSoFar++;
	}
}

