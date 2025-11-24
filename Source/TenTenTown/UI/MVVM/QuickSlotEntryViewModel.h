#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "UObject/WeakObjectPtr.h"
#include "QuickSlotEntryViewModel.generated.h"


class ATTTPlayerState;
class UTexture2D;

UCLASS()
class TENTENTOWN_API UQuickSlotEntryViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
public:
    /**
     * 뷰모델 초기화. ManagerVM에서 호출되며 구독을 시작합니다.
     */
    void Initialize(ATTTPlayerState* InPlayerState, int32 InSlotIndex);

    // ** UI 바인딩 대상 속성들 (FieldNotify는 필수) **

    // 현재 설치 개수 / 최대 설치 개수
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CountText;

    // 설치 비용
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    FText CostText;

    // 아이템 아이콘
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    TObjectPtr<UTexture2D> IconTexture;

    // 현재 보유 재화로 설치 가능한지 여부
    UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "QuickSlot")
    bool bCanAfford = false;

protected:
    TWeakObjectPtr<ATTTPlayerState> PlayerStateWeakPtr;
    int32 SlotIndex = INDEX_NONE; // 이 슬롯이 담당하는 StructureList의 인덱스

    // SlotIndex에 접근할 수 있도록 Getter 추가 (디버그/로직 용)
    // *주의: 이 변수는 public으로 간주하고 cpp에서 사용했습니다.
public:
    int32 GetSlotIndex() const { return SlotIndex; }


protected:
    // ** PlayerState 델리게이트 구독 콜백 함수 **
    UFUNCTION()
    void OnStructureListChanged();

    UFUNCTION()
    void OnGoldChanged(int32 NewGold);

private:
    // 실제 데이터(PS & DB)를 읽어와 UI 속성을 갱신하고 FieldNotify를 발생시킵니다.
    void UpdateAllUIProperties();

    // FieldNotify Setter 함수 정의
    void SetCountText(const FText& NewValue);
    void SetCostText(const FText& NewValue);
    void SetIconTexture(UTexture2D* NewValue);
    void SetCanAfford(bool bNewValue);

public:
    UPROPERTY(BlueprintReadOnly, Category = "QuickSlot")
    int32 SlotNumber = 0; // 초기값 0

    void SetSlotNumber(int32 NewNumber);
};
