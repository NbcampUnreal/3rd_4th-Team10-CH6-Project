// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PCC/PlayPCComponent.h"
#include "UI/PlayWidget.h"
#include "UI/TradeMainWidget.h"
#include "AbilitySystemInterface.h"
#include "Character/GAS/AS/MageAttributeSet/AS_MageAttributeSet.h"
#include "Character/PS/TTTPlayerState.h"
#include "UI/MVVM/PlayerStatusViewModel.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TTTGameplayTags.h"
#include "UI/MVVM/GameStatusViewModel.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "UI/MVVM/PartyManagerViewModel.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "UI/Widget/QuickSlotBarWidget.h"
#include "TimerManager.h"

UPlayPCComponent::UPlayPCComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayPCComponent::BeginPlay()
{
	Super::BeginPlay();

	// [새로운 로직 추가] 맵 이름을 확인하여 불필요한 루프를 방지합니다.
	FString MapName = GetWorld() ? GetWorld()->GetMapName() : FString(TEXT(""));

	// 현재 맵이 로비 맵이라면 (PlayPCC가 불필요함)
	if (MapName.Contains(TEXT("LobbyMap")) || MapName.Contains(TEXT("Lobby")))
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] BeginPlay: In Lobby Map. Deactivating self."));
		// 즉시 비활성화하여 ReBeginPlay 루프가 시작되는 것을 차단합니다.
		Deactivate();
		return;
	}
	ReBeginPlay();
}


void UPlayPCComponent::ReBeginPlay()
{
	// 1. 기존 타이머 정리 (안전을 위해)
	if (GetWorld() && InitCheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(InitCheckTimerHandle);
	}

	// 2. 필수 액터 참조 시도
	APlayerController* PC = GetOwner<APlayerController>();
	if (!PC) return; // Owner PC가 없으면 리턴

	// 이전 레벨의 포인터 정리 (재초기화 시 중요!)
	PlayerStateRef = nullptr;
	GameStateRef = nullptr;
	MyASC = nullptr;

	// 새로운 포인터 가져오기
	ATTTPlayerState* PS = PC->GetPlayerState<ATTTPlayerState>();
	ATTTGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState<ATTTGameStateBase>() : nullptr;
	UAbilitySystemComponent* ASC = PS ? PS->GetAbilitySystemComponent() : nullptr;

	// 3. 유효성 검사 및 대기
	if (PS && GS && ASC)
	{
		// 4. 모든 데이터가 준비됨! (초기화 진행)
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] ReBeginPlay: All GameData Ready! Initializing Gameplay System."));

		// 포인터 캐싱
		PlayerStateRef = PS;
		GameStateRef = GS;
		MyASC = ASC;

		// ViewModels 초기화 (GS, PS, ASC 사용)
		InitializeViewModels();

		// Gameplay Mode Tag 리스너 등록
		// 태그가 부여되면 OnModeTagChanged가 호출되어 HUD를 띄웁니다.
		MyASC->RegisterGameplayTagEvent(GASTAG::State_Mode_Gameplay, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &UPlayPCComponent::OnModeTagChanged);

		// 이미 태그가 붙어있을 경우 (늦게 접속했거나 레벨 이동 직후)
		int32 CurrentCount = MyASC->GetTagCount(GASTAG::State_Mode_Gameplay);
		if (CurrentCount > 0)
		{
			OnModeTagChanged(GASTAG::State_Mode_Gameplay, CurrentCount);
		}
	}
	else
	{
		// 5. 데이터가 아직 미도착 -> 다음 틱에 다시 시도 (재시작 루프)
		UE_LOG(LogTemp, Log, TEXT("[PlayPCC] Waiting for PS/GS/ASC... Retrying ReBeginPlay."));
		// InitCheckTimerHandle을 사용하여 다음 틱에 다시 ReBeginPlay 호출
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPlayPCComponent::ReBeginPlay);
	}
}

// UI 생성 로직: ViewModel 생성 및 Widget에 주입
void UPlayPCComponent::OpenHUDUI()
{

	APlayerController* PC = GetOwner<APlayerController>();
	if (!PC || !PC->IsLocalController() || !PlayWidgetClass || !PlayerStateRef || !GameStateRef)
	{
		if (PC && !PC->IsLocalController())
		{
			UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI Skipped: PC is not Local Controller."));
		}
		return;
	}

	// 1. 뷰모델 생성 (UPlayerStatusViewModel)
	if (!PlayerStatusViewModel)
	{
		// 컴포넌트(this)를 Outer로 사용하여 뷰모델 생성
		PlayerStatusViewModel = NewObject<UPlayerStatusViewModel>(this);

		// 핵심: ViewModel에 PlayerState 데이터 소스를 전달하고 GAS 구독을 시작
		PlayerStatusViewModel->InitializeViewModel(PlayerStateRef);
	}
	if (!GameStatusViewModel)
	{
		GameStatusViewModel = NewObject<UGameStatusViewModel>(this);
		GameStatusViewModel->InitializeViewModel(GameStateRef);
	}
	//퀵슬롯 시스템 초기화
	InitializeQuickSlotSystem();

	// 2. 위젯 생성
	if (!PlayWidgetInstance)
	{
		PlayWidgetInstance = CreateWidget<UPlayWidget>(PC, PlayWidgetClass);
	}

	// 3. 위젯 초기화 및 뷰모델 주입
	if (PlayWidgetInstance)
	{
		//플레이어 스탯 뷰모델 주입
		PlayWidgetInstance->SetPlayerStatusViewModel(PlayerStatusViewModel);
		//게임 상태 뷰모델 주입
		PlayWidgetInstance->SetGameStatusViewModel(GameStatusViewModel);

		if (PartyManagerViewModel)
		{
			// PlayWidget에 UPartyManagerViewModel을 받을 수 있는 함수가 정의되어 있어야 합니다.
			PlayWidgetInstance->SetPartyManagerViewModel(PartyManagerViewModel);
			UE_LOG(LogTemp, Log, TEXT("[PlayPCC] Injected PartyManagerViewModel into PlayWidget."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[PlayPCC] PartyManagerViewModel is NULL! Cannot inject party data."));
		}


		PlayWidgetInstance->AddToViewport();
		PlayWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

// UI 제거 로직
void UPlayPCComponent::CloseHUDUI()
{
	// 1. 뷰모델 정리 (GAS 구독 해제)
	if (PlayerStatusViewModel)
	{
		PlayerStatusViewModel->CleanupViewModel(); // GAS 구독 해제 로직 실행
		PlayerStatusViewModel = nullptr; // GC 대상
	}
	if (GameStatusViewModel)
	{
		GameStatusViewModel->CleanupViewModel(); // GameState 델리게이트 해제 로직 실행
		GameStatusViewModel = nullptr; // GC 대상
	}

	if (PartyManagerViewModel)
	{
		// 내부의 모든 UPartyStatusViewModel의 GAS 구독을 해제합니다.
		PartyManagerViewModel->CleanupViewModel();
		PartyManagerViewModel = nullptr; // GC 대상
	}

	if (QuickSlotManagerViewModel)
	{
		// 퀵슬롯 뷰모델에 별도의 Cleanup 함수가 없더라도,
		// 이 참조를 해제하면 GC 대상이 되며 내부 Entry VM도 정리됩니다.
		QuickSlotManagerViewModel = nullptr;
	}

	// 2. 위젯 제거
	if (PlayWidgetInstance)
	{
		PlayWidgetInstance->RemoveFromParent();
		PlayWidgetInstance = nullptr;
	}

	if (QuickSlotBarWidgetInstance)
	{
		QuickSlotBarWidgetInstance->RemoveFromParent();
		QuickSlotBarWidgetInstance = nullptr;
	}
}

void UPlayPCComponent::InitializeQuickSlotSystem()
{
	APlayerController* PC = GetOwner<APlayerController>();
	// PlayerStateRef는 OpenHUDUI 전에 유효성 검사를 통과했다고 가정합니다.
	ATTTPlayerState* PS = PlayerStateRef;

	//[수정] PlayWidgetInstance가 유효한지 추가로 확인합니다.
	if (!PC || !PS || !PlayWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayPCComponent] QuickSlot 초기화 실패: PC/PS 또는 PlayWidgetInstance가 유효하지 않습니다."));
		return;
	}

	// 1. **Manager ViewModel 생성**
	if (!QuickSlotManagerViewModel)
	{
		QuickSlotManagerViewModel = NewObject<UQuickSlotManagerViewModel>(this);
	}

	// 2. **ViewModel 초기화 (PS 연결 및 Entry VM 9개 생성)**
	QuickSlotManagerViewModel->Initialize(PS);

	// 3.[변경] PlayWidgetInstance에서 QuickSlotBarWidget 인스턴스 가져오기
	// UPlayWidget.h에 UQuickSlotBarWidget* GetQuickSlotBarWidget() const 함수가 정의되어 있어야 합니다.
	UQuickSlotBarWidget* QuickSlotBar = PlayWidgetInstance->GetQuickSlotBarWidget();

	if (QuickSlotBar)
	{
		// QuickSlotBarWidgetInstance 변수에 참조를 저장합니다. (Cleanup을 위해)
		QuickSlotBarWidgetInstance = QuickSlotBar;

		// 4. **Widget 초기화 (ViewModel 주입)**
		QuickSlotBarWidgetInstance->InitializeWidget(QuickSlotManagerViewModel);

		// 5. **화면에 추가** 로직 제거! (PlayWidgetInstance가 이미 띄워졌으므로)

//		UE_LOG(LogTemp, Log, TEXT("[PlayPCComponent] QuickSlot MVVM 시스템 PlayWidget에 통합 완료."));
	}
	else
	{
		// QuickSlotBar가 PlayWidget에 배치되지 않았을 경우
		UE_LOG(LogTemp, Error, TEXT("[PlayPCComponent] PlayWidget 내부에서 QuickSlotBar 위젯을 찾을 수 없습니다. UMG 설정을 확인하십시오."));
	}

	//이전의 QuickSlotBarWidgetClass를 사용하는 로직은 완전히 제거되었습니다.
}


void UPlayPCComponent::InitializeViewModels()
{
	// 1. GameState 가져오기
	ATTTGameStateBase* GameState = Cast<ATTTGameStateBase>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayPCComponent::InitializeViewModels - GameState is not ATTTGameStateBase!"));
		return;
	}

	// 2. PartyManagerViewModel 생성 및 초기화
	if (!PartyManagerViewModel)
	{
		// 뷰모델을 생성합니다. 컴포넌트(this)를 Outer로 지정합니다.
		PartyManagerViewModel = NewObject<UPartyManagerViewModel>(this);
	}

	if (PartyManagerViewModel)
	{
		// BaseViewModel의 InitializeViewModel(void) 함수 대신
		// ATTTGameStateBase*를 받는 오버로드된 함수를 호출합니다.
		// 이 함수는 내부적으로 GameState의 델리게이트를 구독하고 초기 목록을 설정할 것입니다.
		PartyManagerViewModel->InitializeViewModel(GameState);
		UE_LOG(LogTemp, Log, TEXT("PartyManagerViewModel Initialized with GameState."));
	}
}


// Gameplay Tag 변화 핸들러 (HUD On/Off)
void UPlayPCComponent::OnModeTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (Tag.MatchesTag(GASTAG::State_Mode_Gameplay))
	{
		if (NewCount > 0)
		{
			// [수정: 타이머를 사용하여 OpenHUDUI 호출 지연]
			// ASC 초기화 및 AttributeSet 복제 시간을 벌기 위해 짧은 지연을 줍니다.
			GetWorld()->GetTimerManager().SetTimer(
				HUDOpenTimerHandle,
				this,
				&UPlayPCComponent::OpenHUDUI,
				0.1f, // 0.1초 지연
				false);
		}
		else
		{
			// 태그가 제거되면 타이머를 취소하고 UI 닫기
			GetWorld()->GetTimerManager().ClearTimer(HUDOpenTimerHandle);
			CloseHUDUI();
		}
	}

	// TradeMainWidget 관련 태그 로직은 여기에 추가될 수 있습니다.
}

// ----------------------------------------------------------------
// GAS 콜백 함수 제거
// ----------------------------------------------------------------
// 기존의 OnHealthChanged, OnStaminaChanged, OnMaxHealthChanged 함수들은
// UPlayPCComponent.cpp에서 완전히 제거되었습니다.

void UPlayPCComponent::HandlePhaseChanged(ETTTGamePhase NewPhase)
{
	// GameState의 페이즈 변화를 처리
}

void UPlayPCComponent::HandleRemainingTimeChanged(int32 NewRemainingTime)
// GameState의 남은 시간 변화를 처리
{
}

void UPlayPCComponent::RemoveStartWidget()
{
	// 로비나 시작 시 위젯 제거 로직
}

void UPlayPCComponent::OpenTrader(bool BIsOpen)
{
	// 상인 위젯 Open/Close 로직
}