#pragma once

#include "CoreMinimal.h"
#include "UI/MVVM/BaseViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "UObject/WeakObjectPtr.h"
#include "QuickSlotManagerViewModel.generated.h"


class UQuickSlotEntryViewModel;

UCLASS()
class TENTENTOWN_API UQuickSlotManagerViewModel : public UBaseViewModel
{
	GENERATED_BODY()

public:
	/** ManagerVM 초기화. PC 컴포넌트에서 호출됩니다. */
	void Initialize(ATTTPlayerState* InPlayerState);

	/** 퀵슬롯 9개 항목의 뷰모델 배열 (UMG 위젯에 주입될 데이터) */
	UPROPERTY(BlueprintReadOnly, Category = "QuickSlot")
	TArray<TObjectPtr<UQuickSlotEntryViewModel>> QuickSlotEntries;

private:
	// 이 매니저가 바라보는 PlayerState에 대한 약한 참조
	TWeakObjectPtr<ATTTPlayerState> PlayerStateWeakPtr;
	
};
