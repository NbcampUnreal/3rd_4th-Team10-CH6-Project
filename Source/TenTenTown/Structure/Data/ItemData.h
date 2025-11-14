#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct FItemData: public FTableRowBase
{
	GENERATED_BODY()
	
public:
	// 이미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	TSoftObjectPtr<UTexture2D> ItemImage;
	// 최대 보유 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 MaxStackCount;
	// 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FText ItemName;
	// 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FText Description;
	// 판매 가격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	float SellPrice;
	FItemData() : MaxStackCount(5), SellPrice(0.f)
	{
	}
};
