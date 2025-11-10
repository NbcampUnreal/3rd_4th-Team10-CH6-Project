#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlameWallArea.generated.h"

UCLASS()
class TENTENTOWN_API AFlameWallArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlameWallArea();

	void Init(float InLifeTime);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(EditDefaultsOnly, Category="Wall")
	float Lifetime = 5.f;
};
