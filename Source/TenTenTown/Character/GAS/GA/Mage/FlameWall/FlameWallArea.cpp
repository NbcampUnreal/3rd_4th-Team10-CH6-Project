#include "FlameWallArea.h"

AFlameWallArea::AFlameWallArea()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	PrimaryActorTick.bCanEverTick = false;
}

void AFlameWallArea::Init(float InLifeTime)
{
	Lifetime = InLifeTime;
}

void AFlameWallArea::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetLifeSpan(Lifetime);
	}
}

