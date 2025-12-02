// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherBow.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AArcherBow::AArcherBow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
}

// Called when the game starts or when spawned
void AArcherBow::BeginPlay()
{
	Super::BeginPlay();
}

