#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

class TENTENTOWN_API IInteractionInterface
{
	GENERATED_BODY()

public:
	// 상호작용 시 호출 (Instigator: 상호작용을 시도한 캐릭터)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(class ACharacter* InstigatorCharacter);
};
