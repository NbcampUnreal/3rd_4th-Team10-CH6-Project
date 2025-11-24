//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
//#include "InventoryPCComponent.generated.h"
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStructureListChanged);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemListChanged);
//
//
//USTRUCT(BlueprintType)
//struct FInventoryItemData
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FText ItemName;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	int32 Count;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	int32 Level;
//};
//
//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
//class TENTENTOWN_API UInventoryPCComponent : public UActorComponent
//{
//	GENERATED_BODY()
//
//public:
//	UInventoryPCComponent();
//
//protected:
//	virtual void BeginPlay() override;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Gold, meta = (AllowPrivateAccess = true))
//	int32 Gold;
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryStructure, meta = (AllowPrivateAccess = true))
//	TArray<FInventoryItemData> StructureList;
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryItem, meta = (AllowPrivateAccess = true))
//	TArray<FInventoryItemData> ItemList;
//public:
//	UFUNCTION(Server, Reliable, WithValidation)
//	void Server_AddGold(int32 Amount);
//		
//};
