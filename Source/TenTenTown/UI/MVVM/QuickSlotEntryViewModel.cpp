#include "UI/MVVM/QuickSlotEntryViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/Data/StructureData.h"


// --- FieldNotify Setter 구현 (UI에 변경 사항을 알리는 역할) ---

// 매크로 정의 (헤더에 없다고 가정하고 로컬에서 정의)
// #define UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PropertyName) 
// { 
//      BroadcastFieldValueChanged(ThisClass::Get##PropertyName##PropertyName()); 
// }

void UQuickSlotEntryViewModel::SetCountText(const FText& NewValue)
{
	if (!CountText.EqualTo(NewValue))
	{
		CountText = NewValue;

		// 1. UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED 매크로 사용
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CountText);
	}
}
// SetCostText, SetIconTexture, SetCanAfford 도 유사하게 구현되어야 합니다.
// 편의상 아래 로직에서는 직접 값을 설정하고 일괄적으로 브로드캐스트합니다.


// --- Initialize 및 구독 로직 ---

void UQuickSlotEntryViewModel::Initialize(ATTTPlayerState* InPlayerState, int32 InSlotIndex)
{
	if (!InPlayerState || InSlotIndex == INDEX_NONE)
	{
		return;
	}

	PlayerStateWeakPtr = InPlayerState;
	SlotIndex = InSlotIndex;

	// 1. Structure List 변경 델리게이트 구독
	InPlayerState->OnStructureListChangedDelegate.AddDynamic(this, &UQuickSlotEntryViewModel::OnStructureListChanged);

	// 2. Gold 변경 델리게이트 구독 (가격 지불 가능 여부 판단을 위해)
	InPlayerState->OnGoldChangedDelegate.AddDynamic(this, &UQuickSlotEntryViewModel::OnGoldChanged);

	// 3. 최초 상태 갱신
	UpdateAllUIProperties();
}

void UQuickSlotEntryViewModel::OnStructureListChanged()
{
	// StructureList가 변경되면 UI 속성 전체를 갱신합니다.
	UpdateAllUIProperties();
}

void UQuickSlotEntryViewModel::OnGoldChanged(int32 NewGold)
{
	// Gold가 변경되면 설치 가능 여부만 갱신하는 것이 효율적이지만, 여기서는 전체 갱신 호출
	UpdateAllUIProperties();
}

// --- 핵심: DB 조회 및 UI 갱신 로직 ---

void UQuickSlotEntryViewModel::UpdateAllUIProperties()
{
	ATTTPlayerState* PS = PlayerStateWeakPtr.Get();
	UTTTGameInstance* GI = PS ? Cast<UTTTGameInstance>(UGameplayStatics::GetGameInstance(this)) : nullptr;

	if (!PS || !GI || !GI->StructureDataTable) return;

	// 1. PlayerState의 StructureList 데이터 가져오기
	const TArray<FInventoryItemData>& List = PS->GetStructureList();

	if (!List.IsValidIndex(SlotIndex))
	{
		// 이 슬롯 인덱스에 데이터 없음 (이런 일은 초기화가 잘 되었다면 없어야 함)
		return;
	}

	const FInventoryItemData& ItemData = List[SlotIndex];

	// 2. Game Instance의 DB에서 ItemName(FName)으로 Row 조회
	const FStructureData* DBData = GI->StructureDataTable->FindRow<FStructureData>(ItemData.ItemName, TEXT("QuickSlotVM"));

	if (!DBData)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickSlot VM: DB에서 아이템 '%s' (Index %d)를 찾을 수 없습니다."), *ItemData.ItemName.ToString(), SlotIndex);
		// 이 경우에도 UI를 Empty로 설정하는 로직이 필요
		return;
	}

	// 3. UI 속성 갱신 및 FieldNotify 호출

	// CountText 갱신
	FText NewCountText = FText::Format(NSLOCTEXT("QuickSlot", "CountFormat", "{0}/{1}"),
		FText::AsNumber(ItemData.Count),
		FText::AsNumber(DBData->MaxInstallCount)); // MaxCount는 DBData에 있다고 가정
	SetCountText(NewCountText); // FieldNotify 호출

	// CostText 갱신
	FText NewCostText = FText::AsNumber(DBData->InstallCost);
	SetCostText(NewCostText);

	// IconTexture 갱신
	SetIconTexture(DBData->StructureImage.Get());

	// CanAfford 갱신
	bool NewCanAfford = PS->GetGold() >= DBData->InstallCost;
	SetCanAfford(NewCanAfford);

	UE_LOG(LogTemp, Verbose, TEXT("[MVVM] Slot %d 갱신: Count=%d, Gold=%d, CanAfford=%d"), SlotIndex, ItemData.Count, PS->GetGold(), NewCanAfford);
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

void UQuickSlotEntryViewModel::SetSlotNumber(int32 NewNumber)
{
	SlotNumber = NewNumber;
}
