#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickSlotBarWidget.generated.h"


class UQuickSlotManagerViewModel;
class UQuickSlotEntryWidget;

UCLASS()
class TENTENTOWN_API UQuickSlotBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    // 이 위젯이 참조할 Manager ViewModel
    UPROPERTY(BlueprintReadOnly, Category = "MVVM")
    TObjectPtr<UQuickSlotManagerViewModel> ManagerViewModel;

    // UMG에 미리 배치된 9개의 QuickSlotEntryWidget을 바인딩할 변수
    // UMG 블루프린트에서 Is Variable 체크 후 여기에 할당해야 합니다.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_0;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_2;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_3;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_4;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_5;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_6;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_7;    
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UQuickSlotEntryWidget> QuickSlotEntry_8;

    virtual void NativeConstruct() override;

public:
    /**
     * PC/Component에서 Manager ViewModel을 주입하고, 자식 위젯들에 Entry ViewModel을 할당합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void InitializeWidget(UQuickSlotManagerViewModel* InManagerVM);

    UFUNCTION(BlueprintCallable, Category = "UI|QuickSlot")
    void SetQuickSlotManagerViewModel(UQuickSlotManagerViewModel* ManagerVM);

private:
    // UMG 변수를 배열로 묶는 헬퍼 함수
    TArray<UQuickSlotEntryWidget*> GetEntryWidgets() const;
};
