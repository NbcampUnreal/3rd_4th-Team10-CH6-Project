#include "UI/CharSellectWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "UI/WaitHUD.h"

void UCharSellectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (WarriorButton)
	{
		WarriorButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnWarriorButtonClicked);
	}
	if (MageButton)
	{
		MageButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnMageButtonClicked);
	}
	if (ArcherButton)
	{
		ArcherButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnArcherButtonClicked);
	}
	if (RogueButton)
	{
		RogueButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnRogueButtonClicked);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UCharSellectWidget::OnConfirmButtonClicked);
	}
}

void UCharSellectWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
void UCharSellectWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}


void UCharSellectWidget::OnWarriorButtonClicked()
{
}

void UCharSellectWidget::OnMageButtonClicked()
{
}

void UCharSellectWidget::OnArcherButtonClicked()
{
}

void UCharSellectWidget::OnRogueButtonClicked()
{
}

void UCharSellectWidget::OnConfirmButtonClicked()
{
	APlayerController* PlayerController = GetOwningPlayer();

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnOffTradeClicked: PlayerController가 유효하지 않습니다."));
		return;
	}

	AHUD* CurrentHUD = PlayerController->GetHUD();

	if (!CurrentHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnOffTradeClicked: 현재 HUD가 유효하지 않습니다."));
		return;
	}

	AWaitHUD* WaitHUD = Cast<AWaitHUD>(CurrentHUD);

	if (WaitHUD)
	{
		//대기실 페이즈로 보냄
		//WaitHUD->OpenTradeWidget(false);

		UE_LOG(LogTemp, Log, TEXT("OnOffTradeClicked: WaitHUD의 OpenTradeWidget() 호출 성공."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnOffTradeClicked: HUD를 AWaitHUD로 캐스팅하는 데 실패했습니다."));
	}
}
