#include "UI/PlayWidget.h"
#include "UI/MVVM/PlayerStatusViewModel.h"
 #include "UI/MVVM/GameStatusViewModel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PCC/PlayPCComponent.h"
#include "Components/ListView.h"
#include "UI/MVVM/PartyManagerViewModel.h"


void UPlayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 1. UListView* PartyListView가 BindWidget으로 바인딩된 상태인지 확인
    if (!PartyListView)
    {
        UE_LOG(LogTemp, Error, TEXT("PartyListView is not bound in UPlayWidget!"));
        return;
    }

    // 2. PlayerController와 Component 가져오기
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return;

    UPlayPCComponent* PCComponent = PlayerController->FindComponentByClass<UPlayPCComponent>();
    if (!PCComponent) return;

    // 3. Party Manager ViewModel 가져오기
    UPartyManagerViewModel* PartyManagerVM = PCComponent->GetPartyManagerViewModel();

    // 4. UListView에 데이터 바인딩 시작
    if (PartyManagerVM)
    {
        // GetPartyMembers()가 TArray<UObject*>를 상속받은 배열을 반환한다고 가정
        PartyListView->SetListItems(PartyManagerVM->GetPartyMembers());
    }
}

void UPlayWidget::SetPlayerStatusViewModel(UPlayerStatusViewModel* InViewModel)
{
    PlayerStatusViewModel = InViewModel;

    // 이 시점에서 ViewModel이 자식 위젯(예: UPlayerStatusWidget)에도 필요하다면 전달합니다.
}

void UPlayWidget::SetGameStatusViewModel(UGameStatusViewModel* InViewModel)
{
    // GameStatusViewModel이 정의되어 있다면 주석을 해제하고 사용합니다.
    GameStatusViewModel = InViewModel;
}

void UPlayWidget::HideWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UPlayWidget::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);
}

// **MVVM 전환으로 인해 모든 SetXXX 데이터 함수는 제거되었습니다.**