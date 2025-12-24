#include "UI/MVVM/QuickSlotEntryViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/Data/StructureData.h"
#include "UI/PCC/InventoryPCComponent.h"
#include "Engine/World.h"



void UQuickSlotEntryViewModel::InitializeViewModel()
{

}

void UQuickSlotEntryViewModel::CleanupViewModel()
{
	ATTTPlayerState* PS = PlayerStateWeakPtr.Get();
	if (PS)
	{
		//// ⭐⭐ 델리게이트 구독 해제 (메모리 누수 방지) ⭐⭐
		//PS->OnStructureListChangedDelegate.RemoveAll(this);
		//PS->OnGoldChangedDelegate.RemoveAll(this);
	}
	PlayerStateWeakPtr.Reset();
	// 다른 캐싱된 포인터 정리 로직이 있다면 추가합니다.
}


void UQuickSlotEntryViewModel::SetCountText(const FText& NewValue)
{
	if (!CountText.EqualTo(NewValue))
	{
		CountText = NewValue;

		// 1. UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED 매크로 사용
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	}
}
void UQuickSlotEntryViewModel::SetCostText(const FText& NewValue)
{
	if (!CostText.EqualTo(NewValue))
	{
		CostText = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	}
}

void UQuickSlotEntryViewModel::SetIconTexture(UTexture2D* NewValue)
{
	// TSoftObjectPtr::LoadSynchronous()의 결과가 UTexture2D*이므로 포인터 비교
	if (IconTexture != NewValue)
	{
		IconTexture = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	}
}

void UQuickSlotEntryViewModel::SetCanAfford(bool bNewValue)
{
	if (bCanAfford != bNewValue)
	{
		bCanAfford = bNewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanAfford);
	}
}

void UQuickSlotEntryViewModel::SetIsEmptySlot(ESlateVisibility NewValue)
{
	if (bIsEmptySlot != NewValue)
	{
		bIsEmptySlot = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsEmptySlot);
	}
}

void UQuickSlotEntryViewModel::SetSlotNumber(int32 NewNumber)
{
    if (SlotNumber != NewNumber)
    {
        SlotNumber = NewNumber;
    }
}

void UQuickSlotEntryViewModel::OnSetCountTextItem(const FItemData& NewItemData)
{
	MaxCountText = NewItemData.MaxStackCount;
	FText CountTextValue = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));
	SetCountText(CountTextValue);
}

void UQuickSlotEntryViewModel::OnSetCountText(const FStructureData& NewItemData)
{
	MaxCountText = NewItemData.MaxInstallCount;
	FText CountTextValue = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));
	SetCountText(CountTextValue);
}

void UQuickSlotEntryViewModel::SetSlotItem(const FItemData& NewItemData, const FName& RowName)
{
	SlotItemRowName = RowName;
	if (RowName.IsNone())
	{
		SetIsEmptySlot(ESlateVisibility::Collapsed);
	}
	else
	{
		UTexture2D* LoadedTexture = NewItemData.ItemImage.LoadSynchronous();
		if (LoadedTexture)
		{
			UE_LOG(LogTemp, Warning, TEXT("[VM Debug] Icon Texture Loaded and Set: %s"), *LoadedTexture->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[VM Debug] Icon Texture FAILED to Load for Row: %s"), *RowName.ToString());
		}

		SetIconTexture(LoadedTexture);
		SetCostText(FText::AsNumber(NewItemData.SellPrice));
		//SetItemDesText(NewItemData.Description);
		//SetItemName(NewItemData.ItemName);
		OnSetCountTextItem(NewItemData);
		SetIsEmptySlot(ESlateVisibility::Visible);
	}
}

void UQuickSlotEntryViewModel::SetSlotStructure(const FStructureData& NewItemData, const FName& RowName)
{
	SlotItemRowName = RowName;
	if (RowName.IsNone())
	{
		SetIsEmptySlot(ESlateVisibility::Collapsed);
	}
	else
	{
		UTexture2D* LoadedTexture = NewItemData.StructureImage.LoadSynchronous();
		SetIconTexture(LoadedTexture);
		SetCostText(FText::AsNumber(NewItemData.InstallCost));
		//SetItemDesText(NewItemData.Description);
		//SetItemName(NewItemData.ItemName);
		OnSetCountText(NewItemData);
		SetIsEmptySlot(ESlateVisibility::Visible);
	}
}



void UQuickSlotEntryViewModel::UpdateItemDataItem(const TArray<FItemInstance>& NewItemData)
{
	// 이 슬롯이 담당하는 아이템 ID가 유효한지 확인
	if (SlotItemRowName.IsNone())
	{
		return;
	}

	int32 CurrentCount = 0;

	// 1. 인벤토리 배열 전체를 순회하며 이 슬롯의 아이템 개수를 합산
	for (const FItemInstance& ItemInstance : NewItemData)
	{
		// ItemInstance 구조체에 ItemID 필드가 있다고 가정
		if (ItemInstance.ItemID == SlotItemRowName)
		{
			CurrentCount += ItemInstance.Count;
		}
	}
	CurrentCountText = CurrentCount;
	CountText = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
}

void UQuickSlotEntryViewModel::UpdateItemData(const TArray<FInventoryItemData>& NewItemData)
{
	// 이 슬롯이 담당하는 아이템 ID가 유효한지 확인
	if (SlotItemRowName.IsNone())
	{
		return;
	}

	int32 CurrentCount = 0;

	// 1. 인벤토리 배열 전체를 순회하며 이 슬롯의 아이템 개수를 합산
	for (const FInventoryItemData& ItemInstance : NewItemData)
	{
		// ItemInstance 구조체에 ItemID 필드가 있다고 가정
		if (ItemInstance.ItemName == SlotItemRowName)
		{
			CurrentCount += ItemInstance.Count;
		}
	}

	// 2. ViewModel의 Count 프로퍼티를 갱신하고 View에 알림을 보냅니다.
	// (여기서는 GetCurrentCountText() 함수가 이 Count를 사용한다고 가정합니다.)

	// 만약 Count를 직접 저장하는 변수가 있다면:
	CurrentCountText = CurrentCount;
	CountText = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCountText, MaxCountText));

	// 3. View에 변경 알림 (MVVM FieldNotify)
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);

	////-------
	////인스턴스 겟
	//UWorld* World = GetWorld();

	//if (!World) { return; }
	//// UGameplayStatics::GetGameInstance<T>를 사용하여 안전하게 캐스팅하여 가져옵니다.
	//UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>();
	//if (!GI) { return; }

	////게임인스턴스의 StructureDataTable를 가져옴 거기서 FStructureData Row를 찾음	
	//const FStructureData* StructureData = GI->StructureDataTable->FindRow<FStructureData>(FName(*NewData.ItemName.ToString()), TEXT("QuickSlotEntryVM"));
	//
	////세팅
	//if (!StructureData)
	//{
	//	// 데이터가 없는 경우 뷰모델 필드를 기본값/빈 값으로 설정하고 종료합니다.
	//	SetIsEmptySlot(ESlateVisibility::Collapsed);
	//	/*SetIconTexture(nullptr);
	//	SetCostText(FText::GetEmpty());
	//	SetCountText(FText::GetEmpty());*/
	//	return;
	//}


	/*SetIconTexture(StructureData->StructureImage.LoadSynchronous());

	FText CostTextValue = FText::Format(NSLOCTEXT("QuickSlot", "InstallCost", "{0} G"), FText::AsNumber(StructureData->InstallCost));
	SetCostText(CostTextValue);

	FText CountTextValue = FText::FromString(FString::Printf(TEXT("%d / %d"), NewData.Count, StructureData->MaxInstallCount));
	SetCountText(CountTextValue);*/
	
}

void UQuickSlotEntryViewModel::ClearItemData()
{
	SetIconTexture(nullptr);
	SetCostText(FText::GetEmpty());
	SetCountText(FText::GetEmpty());
}

void UQuickSlotEntryViewModel::BroadcastAllFieldValues()
{
	// 필드 알림으로 설정된 모든 UPROPERTY에 대해 호출하여 초기 데이터를 View에 전달합니다.
	UE_LOG(LogTemp, Log, TEXT("UQuickSlotEntryViewModel::BroadcastAllFieldValues - Initial data push."));

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanAfford);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(SlotNumber);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bIsEmptySlot);
}

