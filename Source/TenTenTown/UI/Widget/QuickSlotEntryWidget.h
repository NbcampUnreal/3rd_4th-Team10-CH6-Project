#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotEntryWidget.generated.h"


class UQuickSlotEntryViewModel;
class UButton;

UCLASS()
class TENTENTOWN_API UQuickSlotEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    // UMG 위젯에 바인딩될 변수 (Is Variable 체크되어야 함)
    // 이 변수를 통해 뷰모델의 FieldNotify 속성에 접근합니다.
    UPROPERTY(BlueprintReadOnly, Category = "MVVM")
    TObjectPtr<UQuickSlotEntryViewModel> EntryViewModel;

    // UMG 내부의 버튼 (클릭 이벤트 처리를 위해)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuickSlotButton;

    // UUserWidget의 NativeConstruct 오버라이드 (초기화 로직)
    virtual void NativeConstruct() override;

public:
    /**
     * Manager Widget에서 호출하여 ViewModel 인스턴스를 주입합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetViewModel(UQuickSlotEntryViewModel* InViewModel);

protected:
    // 버튼 클릭 이벤트 핸들러 (선택 로직)
    UFUNCTION()
    void OnQuickSlotClicked();


};
