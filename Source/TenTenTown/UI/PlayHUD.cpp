#include "UI/PlayHUD.h"
#include "UI/PlayWidget.h"
#include "UI/TradeMainWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "AbilitySystemInterface.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"
#include "Character/PS/TTTPlayerState.h"
#include "Structure/Data/StructureData.h"
#include "Structure/Data/ItemData.h"




void APlayHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayWidgetClass)
	{	
		PlayWidgetInstance = CreateWidget<UPlayWidget>(GetWorld(), PlayWidgetClass);
		if (PlayWidgetInstance)
		{
			PlayWidgetInstance->AddToViewport();			
		}
		else
		{
			return;
		}
	}
	if (TradeWidgetClass)
	{
		TradeWidgetInstance = CreateWidget<UTradeMainWidget>(GetWorld(), TradeWidgetClass);
		if (TradeWidgetInstance)
		{
			TradeWidgetInstance->AddToViewport();
			TradeWidgetInstance->HideWidget();
		}
		else
		{
			return;
		}
	}


	StartWidgetSetting();
}



void APlayHUD::StartWidgetSetting()
{
	APlayerController* PC = GetOwningPlayerController();
    
	if (PC)
	{
		//ASC
		if (!bSettingArray[0])
		{
			IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(PC->PlayerState);

			if (ASCInterface)
			{
				MyASC = ASCInterface->GetAbilitySystemComponent();

				if (MyASC)
				{
					bSettingArray[0] = true;
					SettingCount++;
					SetBindDelegates();
				}
			}
		}
		//게임 스테이트
		if (!bSettingArray[1])
		{
			GameStateRef = PC->GetWorld()->GetGameState<ATTTGameStateBase>();
			if (GameStateRef)
			{
				GameStateRef->OnPhaseChanged.AddDynamic(this, &APlayHUD::HandlePhaseChanged);
				GameStateRef->OnRemainingTimeChanged.AddDynamic(this, &APlayHUD::HandleRemainingTimeChanged);

				HandlePhaseChanged(GameStateRef->Phase);
				HandleRemainingTimeChanged(GameStateRef->RemainingTime);

				bSettingArray[1] = true;
				SettingCount++;
			}
		}		
		//플레이어 스테이트
		if (!bSettingArray[2])
		{
			PlayerStateRef = Cast<ATTTPlayerState>(PC->PlayerState);
			if (PlayerStateRef)
			{
				//델리게이트 바인딩
				PlayerStateRef->OnGoldChangedDelegate.AddDynamic(this, &APlayHUD::OnPlayerGoldChanged);
				PlayerStateRef->OnStructureListChangedDelegate.AddDynamic(this, &APlayHUD::OnPlayerInventoryStructureChanged);
				PlayerStateRef->OnItemListChangedDelegate.AddDynamic(this, &APlayHUD::OnPlayerInventoryItemChanged);
								
				//OnPlayerGoldChanged(PlayerStateRef->Gold); // 초기 골드 값 반영
				// 초기 인벤토리 반영
				OnPlayerInventoryStructureChanged();
				OnPlayerInventoryItemChanged();

				bSettingArray[2] = true;
				SettingCount++;
			}
		}
	}



	if (SettingCount < 3)
	{
		FTimerHandle RetryTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RetryTimerHandle, this, &APlayHUD::StartWidgetSetting, 0.2f, false);
	}
}


void APlayHUD::SetBindDelegates()
{
	if (!MyASC) return;

	const UAS_FighterAttributeSet* FighterAttributes = MyASC->GetSet<UAS_FighterAttributeSet>();

	if (FighterAttributes)
	{
		MyASC->GetGameplayAttributeValueChangeDelegate(FighterAttributes->GetHealthAttribute())
			.AddUObject(this, &APlayHUD::OnHealthChanged);

		MyASC->GetGameplayAttributeValueChangeDelegate(FighterAttributes->GetMaxHealthAttribute())
			.AddUObject(this, &APlayHUD::OnHealthChanged);
	}
}

void APlayHUD::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!MyASC) return;

	const UAS_FighterAttributeSet* FighterAttributes = MyASC->GetSet<UAS_FighterAttributeSet>();

	if (FighterAttributes)
	{
		float CurrentHealth = FighterAttributes->GetHealth();
		float MaxHealthValue = FighterAttributes->GetMaxHealth();

		float HealthPercent = (MaxHealthValue > 0.0f)
			? (CurrentHealth / MaxHealthValue)
			: 0.0f;

		//PlayWidgetInstance->SetHealthPercent(HealthPercent);

		//텍스트 블록 등이 있다면 여기서 업데이트
		// HealthTextBlock->SetText(FText::Format(LOCTEXT("HealthFormat", "{0}/{1}"), FMath::FloorToInt(CurrentHealth), FMath::FloorToInt(MaxHealthValue)));
	}
}

void APlayHUD::HandlePhaseChanged(ETTTGamePhase NewPhase)
{
	if (NewPhase == ETTTGamePhase::Waiting)
	{

	}
	else if(NewPhase == ETTTGamePhase::Build)
	{

	}
	else if (NewPhase == ETTTGamePhase::Combat)
	{
		TradeWidgetInstance->HideWidget();
		PlayWidgetInstance->ShowWidget();
	}
	else if (NewPhase == ETTTGamePhase::Reward)
	{

	}
	else if (NewPhase == ETTTGamePhase::Victory)
	{
		TradeWidgetInstance->HideWidget();
		PlayWidgetInstance->HideWidget();
	}
	else if (NewPhase == ETTTGamePhase::GameOver)
	{
		TradeWidgetInstance->HideWidget();
		PlayWidgetInstance->HideWidget();
	}
}

void APlayHUD::HandleRemainingTimeChanged(int32 NewRemainingTime)
{
	//PlayWidgetInstance->SetWaveTimer(NewRemainingTime);
}



void APlayHUD::OnPlayerGoldChanged(int32 NewGold)
{
	if (PlayWidgetInstance)
	{
		//PlayWidgetInstance->SetMoneyText(NewGold);
	}
	if (TradeWidgetInstance)
	{
		TradeWidgetInstance->SetMoneyText(NewGold);
	}
}

void APlayHUD::OnPlayerInventoryStructureChanged()
{
	if (PlayWidgetInstance && PlayerStateRef)
	{
		// PlayWidgetInstance의 함수를 호출하여 PlayerStateRef->InventoryList 전체를 읽고 UI를 재구성합니다.
		// 예: PlayWidgetInstance->RebuildInventoryUI(PlayerStateRef->InventoryList);
		UE_LOG(LogTemp, Warning, TEXT("HUD: Inventory Changed (Needs Rebuild)"));
	}
}
void APlayHUD::OnPlayerInventoryItemChanged()
{
	if (PlayWidgetInstance && PlayerStateRef)
	{
		
	}
}

void APlayHUD::OpenTradeWidget(bool bIsOpen)
{
	if (bIsOpen)
	{
		TradeWidgetInstance->ShowWidget();
		PlayWidgetInstance->HideWidget();
	}
	else
	{
		TradeWidgetInstance->HideWidget();
		PlayWidgetInstance->ShowWidget();
	}
}

//게임모드에서 실행시켜서 스크롤 설정
void APlayHUD::SetTradeScroll()
{
	
	//ps의 정보와 게임모드에 잇는 데이터테이블을 이용하여 스크롤 세팅
	//지금은 임시로 데이터테이블 여기서 사용한다 StructureDataTable  ItemDataTable
	//그리고 구조물과 아이템 스크롤을 가져온다.
	//add함수를 사용해서 순차적으로 배치한다.

	APlayerController* PC = GetOwningPlayerController();
	if (!PlayerStateRef) return;
	if (!TradeWidgetInstance) return;
	if (!StructureDataTable) return;
	
	//구조물 처리
	TArray<FStructureData*> RowArray;
	StructureDataTable->GetAllRows<FStructureData>(TEXT("ProcessAllStructureData Context"), RowArray);
		
	for (const FStructureData* RowData : RowArray)
	{
		if (RowData)
		{
			USlotWidget* NewSlot = TradeWidgetInstance->GetTraderWidget(1)->GetScrollWidgets()->GetAddSlot();

			UTexture2D* LoadedTexture = RowData->StructureImage.LoadSynchronous();
			int32 PSlevel = PlayerStateRef->FindStructureDataByName(RowData->StructureName)->Level;
			FText HeadText;
			if (PSlevel == 0)
			{
				HeadText = FText::FromString(TEXT("-"));
			}
			else if (PSlevel >= RowData->MaxUpgradeLevel)
			{
				HeadText = FText::FromString(TEXT("Max"));
			}
			else
			{
				FFormatOrderedArguments Args;
				Args.Add(FText::AsNumber(PSlevel));         // 현재 레벨
				Args.Add(FText::AsNumber(RowData->MaxUpgradeLevel)); // 최대 레벨

				HeadText = FText::Format(
					NSLOCTEXT("StructureUI", "LevelProgressFormat", "{0}/{1}"),
					Args
				);
			}
			int32 NewPrice = (PSlevel < RowData->MaxUpgradeLevel) ? RowData->UpgradeCosts[PSlevel] : -1;
			

			//NewSlot->SetSlotWidgetData(RowData->StructureName, LoadedTexture, RowData->StructureName, NewPrice, -1);
		}
	}

	if (!ItemDataTable) return;
	
	//아이템 처리
	TArray<FItemData*> RowArrayItem;
	ItemDataTable->GetAllRows<FItemData>(TEXT("ProcessAllStructureData Context"), RowArrayItem);

	for (const FItemData* RowData : RowArrayItem)
	{
		if (RowData)
		{
			USlotWidget* NewSlot = TradeWidgetInstance->GetTraderWidget(2)->GetScrollWidgets()->GetAddSlot();

			UTexture2D* LoadedTexture = RowData->ItemImage.LoadSynchronous();
			//int32 PSlevel = PlayerStateRef->FindItemDataByName(RowData->ItemName)->Level;
			int32 PSCount = PlayerStateRef->FindItemDataByName(RowData->ItemName)->Count;

			//NewSlot->SetSlotWidgetData(RowData->ItemName, LoadedTexture, FText::AsNumber(PSCount), RowData->SellPrice, -1);
		}
	}

	//TraderWidget 대표 슬롯 초기 설정
	//FText FirstNameS = TradeWidgetInstance->GetTraderWidget(1)->GetScrollWidgets()->GetSlot(0)->GetDataName();
	//FText FirstNameI = TradeWidgetInstance->GetTraderWidget(2)->GetScrollWidgets()->GetSlot(0)->GetDataName();

	//TradeWidgetInstance->GetTraderWidget(1)->ChangeHeadSlot(FirstNameS);
	//TradeWidgetInstance->GetTraderWidget(2)->ChangeHeadSlot(FirstNameI);
	
}

