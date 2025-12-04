#include "UI/TradeMainWidget.h"
#include "Components/Button.h"
#include "UI/PlayHUD.h"
#include "Components/TextBlock.h"
#include "Components/ListView.h"



void UTradeMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UTradeMainWidget::SetPlayerStatusViewModel(UPlayerStatusViewModel* ViewModel)
{
    PlayerStatusViewModel = ViewModel;
}

void UTradeMainWidget::SetTradeViewModel(UTradeViewModel* ViewModel)
{
    UE_LOG(LogTemp, Warning, TEXT("bbbbbbbbbbbbbbbbbbbb"));
    // 1. ViewModel 참조 저장
    TradeViewModel = ViewModel;

    // 2. ViewModel이 유효하고 UListView가 바인딩되어 있다면 목록 설정
    if (TradeViewModel && TradeListView)
    {
        //조건 충족
        UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaa"));

        // 이 시점에 PartyManagerViewModel은 이미 GameState를 구독하고 초기 목록을 가지고 있습니다.
        // UListView의 SetListItems 함수를 사용하여 목록 데이터를 주입합니다.
        TradeListView->SetListItems(TradeViewModel->GetPartyMembers());

        TradeViewModel->CallSlotDelegate();

        // 3. 목록이 동적으로 변경될 때 UListView를 갱신하는 옵션 설정 (선택 사항)
        // SetIsRefreshable(true)는 FFieldNotify를 사용하여 목록이 변경될 때 UListView를 자동으로 갱신합니다.
        // UPROPERTY(BlueprintReadOnly, Category = "MVVM") TObjectPtr<UPartyManagerViewModel> PartyManagerViewModel;
        // 이 속성에 FieldNotify가 설정되어 있다면, PartyListView->SetListItems(PartyManagerViewModel->GetPartyMembers())만으로 충분할 수 있습니다.
    }
    else if (!TradeListView)
    {
		UE_LOG(LogTemp, Error, TEXT("[UTradeMainWidget] TradeListView is not bound!"));
    }
}


