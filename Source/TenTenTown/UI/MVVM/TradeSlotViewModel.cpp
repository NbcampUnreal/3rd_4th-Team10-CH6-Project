#include "UI/MVVM/TradeSlotViewModel.h"


void UTradeSlotViewModel::SetBindingDelegate()
{

}

void UTradeSlotViewModel::SetSlotItem(const FItemData& NewItemData)
{
	UE_LOG(LogTemp, Log, TEXT("bbbbUTradeSlotViewModel::SetSlotItem called for item: %s"), *NewItemData.ItemName.ToString());
	ItemName = NewItemData.ItemName;
	SetIconTexture(NewItemData.ItemImage.Get());
	SetCostText(FText::AsNumber(NewItemData.SellPrice));

	OnSetCountText(NewItemData);
}

void UTradeSlotViewModel::OnSetCountText(const FItemData& NewItemData)
{
	FText CountTextValue = FText::FromString(FString::Printf(TEXT("%d / %d"), 0, NewItemData.MaxStackCount));
	SetCountText(CountTextValue);
}

void UTradeSlotViewModel::SetCountText(const FText& NewValue)
{
	if (!CountText.EqualTo(NewValue))
	{
		CountText = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	}
}
void UTradeSlotViewModel::SetCostText(const FText& NewValue)
{
	UE_LOG(LogTemp, Log, TEXT("UTradeSlotViewModel::SetCostText called. NewValue: %s"), *NewValue.ToString());
	CostText = NewValue;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	/*if (!CostText.EqualTo(NewValue))
	{
		CostText = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	}*/
}

void UTradeSlotViewModel::SetIconTexture(UTexture2D* NewValue)
{
	// TSoftObjectPtr::LoadSynchronous()의 결과가 UTexture2D*이므로 포인터 비교
	if (IconTexture != NewValue)
	{
		IconTexture = NewValue;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	}
}

//void UTradeSlotViewModel::SetCanAfford(bool bNewValue)
//{
//	if (bCanAfford != bNewValue)
//	{
//		bCanAfford = bNewValue;
//		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanAfford);
//	}
//}

//void UTradeSlotViewModel::UpdateItemData(const FInventoryItemData& NewData)
//{
//	//인스턴스 겟
//	UWorld* World = GetWorld();
//
//	if (!World) { return; }
//	// UGameplayStatics::GetGameInstance<T>를 사용하여 안전하게 캐스팅하여 가져옵니다.
//	UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>();
//	if (!GI) { return; }
//
//	//게임인스턴스의 StructureDataTable를 가져옴 거기서 FStructureData Row를 찾음	
//	const FStructureData* StructureData = GI->StructureDataTable->FindRow<FStructureData>(FName(*NewData.ItemName.ToString()), TEXT("QuickSlotEntryVM"));
//
//	//세팅
//	if (!StructureData)
//	{
//		// 데이터가 없는 경우 뷰모델 필드를 기본값/빈 값으로 설정하고 종료합니다.
//		SetIconTexture(nullptr);
//		SetCostText(FText::GetEmpty());
//		SetCountText(FText::GetEmpty());
//		return;
//	}
//
//
//	SetIconTexture(StructureData->StructureImage.LoadSynchronous());
//
//	FText CostTextValue = FText::Format(NSLOCTEXT("QuickSlot", "InstallCost", "{0} G"), FText::AsNumber(StructureData->InstallCost));
//	SetCostText(CostTextValue);
//
//	
//
//}

void UTradeSlotViewModel::BroadcastAllFieldValues()
{
	// 필드 알림으로 설정된 모든 UPROPERTY에 대해 호출하여 초기 데이터를 View에 전달합니다.	
	UE_LOG(LogTemp, Log, TEXT("UTradeSlotViewModel::BroadcastAllFieldValues - Initial data push."));

	//CostText를 로그로 표시해
	UE_LOG(LogTemp, Log, TEXT("Broadcasting CostText: %s"), *CostText.ToString());



	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CostText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bCanAfford);	
}
