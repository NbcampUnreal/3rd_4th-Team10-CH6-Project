#include "UI/PartyWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TraderWidget.h"
#include "UI/MVVM/PartyStatusViewModel.h"
#include "Blueprint/IUserObjectListEntry.h"



//void UPartyWidget::SetTargetItemImage(UTexture2D* HeadTexture)
//{
//	HeadImage->SetBrushFromTexture(HeadTexture);
//}
//
//void UPartyWidget::SetHealthBar(float HealthPer)
//{
//	HealthBar->SetPercent(HealthPer);
//}
//
//void UPartyWidget::SetNameText(FText NewText)
//{
//	NameText->SetText(NewText);
//}
//void UPartyWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
//{
//    //Super::NativeOnListItemObjectSet(ListItemObject);
//
//    // 1. 전달받은 UObject를 우리가 예상하는 ViewModel 타입으로 캐스팅합니다.
//    UPartyStatusViewModel* AssignedViewModel = Cast<UPartyStatusViewModel>(ListItemObject);
//
//    if (AssignedViewModel)
//    {
//        // 2. 캐스팅에 성공했다면, 이 위젯이 바인딩된 정보를 로그로 출력합니다.
//        // GetNameText() 함수가 UPartyStatusViewModel에 구현되어 있다고 가정합니다.
//        FString PlayerName = AssignedViewModel->GetNameText().ToString();
//
//        UE_LOG(LogTemp, Warning,
//            TEXT("[PartyWidget Binding] Widget created/reused and bound to Player: %s (VM Hash: %d)"),
//            *PlayerName,
//            AssignedViewModel->GetUniqueID());
//
//        // 3. ViewModel 참조를 저장합니다.
//        PartyStatusViewModel = AssignedViewModel;
//
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT("[PartyWidget Binding] Received NULL or incorrect object type for list item."));
//    }
//}

void UPartyWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UE_LOG(LogTemp, Warning, TEXT("[PartyWidget] NativeOnListItemObjectSet called."));
    // 부모 클래스 함수 호출
    //Super::NativeOnListItemObjectSet(ListItemObject);

    // 1. 전달받은 UObject를 캐스팅하여 위젯의 변수에 할당합니다. <--- 이 코드가 필수입니다!
    PartyStatusViewModel = Cast<UPartyStatusViewModel>(ListItemObject);

    if (PartyStatusViewModel)
    {
        // 2. 바인딩이 작동하기 위한 초기 데이터 설정 (선택적이지만 권장)
        // 예: NameText를 즉시 설정하여 바인딩 시스템에 부하를 줄일 수 있습니다.
        // NameText->SetText(PartyStatusViewModel->GetNameText());
    }
}
