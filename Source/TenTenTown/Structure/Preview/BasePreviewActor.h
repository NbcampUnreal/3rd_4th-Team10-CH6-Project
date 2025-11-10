#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePreviewActor.generated.h"

UCLASS()
class TENTENTOWN_API ABasePreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasePreviewActor();
	virtual void Tick(float DeltaTime) override;

protected:
	// 설치 가능한 최대 거리
	UPROPERTY(EditAnywhere)
	float MaxInstallDistance = 600.f;

	// 라인 트레이스가 감지할 표면
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel3;
};
