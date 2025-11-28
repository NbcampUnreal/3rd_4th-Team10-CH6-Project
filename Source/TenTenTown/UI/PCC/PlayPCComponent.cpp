#include "PlayPCComponent.h"
#include "GameFramework/PlayerController.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameSystem/GameMode/TTTGameStateBase.h"
#include "AbilitySystemInterface.h"
#include "Components/PrimitiveComponent.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "UI/PlayWidget.h"
#include "UI/TradeMainWidget.h"
#include "UI/Widget/QuickSlotBarWidget.h"
#include "UI/MVVM/PlayerStatusViewModel.h"
#include "UI/MVVM/QuickSlotManagerViewModel.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "TTTGameplayTags.h" 



UPlayPCComponent::UPlayPCComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 뷰모델 생성 (생성자에서만 수행)
    /*PlayerStatusViewModel = CreateDefaultSubobject<UPlayerStatusViewModel>(TEXT("PlayerStatusViewModel"));
    GameStatusViewModel = CreateDefaultSubobject<UGameStatusViewModel>(TEXT("GameStatusViewModel"));
    PartyManagerViewModel = CreateDefaultSubobject<UPartyManagerViewModel>(TEXT("PartyManagerViewModel"));
    QuickSlotManagerViewModel = CreateDefaultSubobject<UQuickSlotManagerViewModel>(TEXT("QuickSlotManagerViewModel"));*/
    
    /*PlayerStatusViewModel = NewObject<UPlayerStatusViewModel>();
    GameStatusViewModel = NewObject<UGameStatusViewModel>();
    PartyManagerViewModel = NewObject<UPartyManagerViewModel>();
    QuickSlotManagerViewModel = NewObject<UQuickSlotManagerViewModel>();*/
	UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] Constructor: ViewModels Created."));
}

void UPlayPCComponent::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] BeginPlay called."));
    Super::BeginPlay();
    
    PlayerStatusViewModel = NewObject<UPlayerStatusViewModel>();
    GameStatusViewModel = NewObject<UGameStatusViewModel>();
    PartyManagerViewModel = NewObject<UPartyManagerViewModel>();
    QuickSlotManagerViewModel = NewObject<UQuickSlotManagerViewModel>();
    

    
    APlayerController* PC = GetPlayerController();
    //if (!PC->IsLocalController())
    //{
    //    // 로컬 플레이어가 아니므로 로직을 수행하지 않고 종료
    //    return;
    //}
    // BeginPlay에서는 PC/PS/GS 등이 아직 유효하지 않으므로, ReBeginPlay를 호출하여 기다립니다.
    //GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPlayPCComponent::ReBeginPlay);
    //GetWorld()->GetTimerManager().SetTimer(ReBeginPlayTimerHandle, this, &UPlayPCComponent::ReBeginPlay, 7.0f, false, -1.0f);

    GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandle,
        this,
        &UPlayPCComponent::TestFunction,
        0.1f, // 0.1초 뒤에 실행
        false // 반복 안 함 (한 번만 실행)
    );
    
    
}

void UPlayPCComponent::TestFunction()
{
    if (!MyASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] BeginPlay: MyASC is null, calling FindSetASC()"));
        FindSetASC();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] BeginPlay: MyASC already set."));
    }
}

void UPlayPCComponent::FindSetASC()
{
    APlayerController* PC = GetPlayerController();
    ATTTPlayerState* PS = GetPlayerStateRef();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] FindSetASC: Attempting to find ASC..."));
    if (ASC)
    {
        MyASC = ASC;
        UE_LOG(LogTemp, Warning, TEXT("aaaASC: %p"), MyASC.Get());
        // ⭐ 1. 이벤트 구독 (기존 코드)
        const FGameplayTag ModeTag = GASTAG::State_Mode_Gameplay; // 가독성을 위해 태그를 변수에 저장합니다.

        MyASC->RegisterGameplayTagEvent(ModeTag, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &UPlayPCComponent::OnModeTagChanged);


        UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] FindSetASC: ASC Found and Registered Tag Event."));

        // ---------------------------------------------------------------------------------
        // ⭐ 2. 즉시 실행 (초기화 로직)

        // 현재 태그 카운트 조회 (0 또는 1)
        int32 CurrentCount = MyASC->GetTagCount(ModeTag);

        // OnModeTagChanged 함수를 현재 카운트로 직접 호출
        // (⚠️ 주의: OnModeTagChanged 함수의 실제 인자 시그니처에 맞춰서 호출해야 합니다.)

        // 만약 함수 시그니처가 'void OnModeTagChanged(int32 NewCount)'라면:
        OnModeTagChanged(ModeTag, CurrentCount);

        // 만약 표준 시그니처 'void OnModeTagChanged(FGameplayTag Tag, int32 NewCount)'라면:
        // OnModeTagChanged(ModeTag, CurrentCount);
        
	}
    else
    {
        GetWorld()->GetTimerManager().SetTimer(
            SetASCTimerHandle,
            this,
            &UPlayPCComponent::FindSetASC,
            0.1f, // 0.1초 뒤에 실행
            false // 반복 안 함 (한 번만 실행)
        );
    }
}

void UPlayPCComponent::OnModeTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (Tag == GASTAG::State_Mode_Gameplay)
    {
		UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OnModeTagChanged called with NewCount: %d"), NewCount);
        if (NewCount > 0)
        {
			UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] Gameplay Mode Entered - Opening HUD UI."));
            OpenHUDUI();
        }
        else
        {
            GetWorld()->GetTimerManager().ClearTimer(OpenReadyTimerHandle);
            CloseHUDUI();
        }
    }
}


void UPlayPCComponent::OpenHUDUI()
{
	UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI called. Checking AttributeSet readiness..."));
    APlayerController* PC = GetPlayerController();
    ATTTPlayerState* PS = GetPlayerStateRef();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    
    //캐릭터 없으면 리턴
    if (!ASC || !ASC->GetAttributeSet(UAS_CharacterBase::StaticClass()))
    {
        CallOpenReadyUI();
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI: AttributeSet is Ready. Proceeding to Open HUD UI."));
    ATTTGameStateBase* GS = GetGameStateRef();
        
    if (!PC || !PC->GetPawn() || !PS || !GS)
    {
        CallOpenReadyUI();
        return;
    }
	UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] OpenHUDUI: Essential references are valid. Proceeding to DelayedOpenHUDUI."));
    PlayerStateRef = PS;
    GameStateRef = GS;
    MyASC = ASC;
    UE_LOG(LogTemp, Warning, TEXT("bbbASC: %p"), MyASC.Get());

	UE_LOG(LogTemp, Log, TEXT("[PlayPCC] All essential references are valid. Proceeding with HUD UI initialization."));

    
    // 2. ViewModel 초기화
   
    PlayerStatusViewModel->InitializeViewModel(PlayerStateRef, MyASC.Get());
    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] PlayerStatusViewModel initialized."));

    GameStatusViewModel->InitializeViewModel(GameStateRef, MyASC.Get());
    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] GameStatusViewModel initialized."));

    PartyManagerViewModel->InitializeViewModel(PlayerStateRef, GameStateRef);
    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] PartyManagerViewModel Initialized in DelayedOpenHUDUI."));
    

    // 3. PlayWidgetInstance 생성
    if (!PlayWidgetInstance)
    {
        PlayWidgetInstance = CreateWidget<UPlayWidget>(PC, PlayWidgetClass);
    }

    // ⭐ PlayWidgetInstance 유효성 검증
    if (!PlayWidgetInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayPCC] PlayWidgetInstance FAILED TO CREATE. Aborting UI Init."));
        return;
    }




    // 4. QuickSlot 초기화 (PlayWidgetInstance 유효성 보장 후 호출)
    InitializeQuickSlotSystem();


    // 5. ViewModel 주입 및 화면 표시
    PlayWidgetInstance->SetPlayerStatusViewModel(PlayerStatusViewModel);
    PlayWidgetInstance->SetGameStatusViewModel(GameStatusViewModel);
    PlayWidgetInstance->SetPartyManagerViewModel(PartyManagerViewModel);

    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] All ViewModels Injected into PlayWidget."));

    // 파티 리스트 뷰 바인딩 및 초기화 로직이 PlayWidget 내부에서 호출되는 함수라고 가정
    

    // 임시지연
    //PlayWidgetInstance->SetsPartyListView();
    GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandle3,
        this,
        &UPlayPCComponent::TestFunction3,
        10.0f,
        false
    );



    if (!PlayWidgetInstance->IsInViewport())
    {
        PlayWidgetInstance->AddToViewport();
        UE_LOG(LogTemp, Log, TEXT("[PlayPCC] PlayWidget added to viewport."));
    }

 /*   GetWorld()->GetTimerManager().SetTimer(
        RefreshTimerHandle,
        this,
        &UPlayPCComponent::ForceRefreshList,
        5.1f,
        false
    );*/

    GetWorld()->GetTimerManager().SetTimer(
        TestTimerHandle2,
        this,
        &UPlayPCComponent::TestFunction2,
        1.0f,
        true
    );
}

void UPlayPCComponent::CallOpenReadyUI()
{
    GetWorld()->GetTimerManager().SetTimer(
        OpenReadyTimerHandle,
        this,
        &UPlayPCComponent::OpenHUDUI,
        0.1f,
        false
    );
}

void UPlayPCComponent::ForceRefreshList()
{
    if (!GameStateRef)
    {
        return;
    }

    GetPartyManagerViewModel()->ResetAndRefreshAll();
}




void UPlayPCComponent::TestFunction3()
{
    PlayWidgetInstance->SetPartyManagerViewModel(PartyManagerViewModel);
    //PlayWidgetInstance->SetsPartyListView();
}




void UPlayPCComponent::RemoveStartWidget()
{
    // PlayWidgetInstance를 제거하는 로직 (필요 시 구현)
}

void UPlayPCComponent::OpenTrader(bool BIsOpen)
{
    // TradeMainWidget을 열고 닫는 로직 (필요 시 구현)
}





void UPlayPCComponent::CloseHUDUI()
{
    if (PlayWidgetInstance && PlayWidgetInstance->IsInViewport())
    {
        PlayWidgetInstance->RemoveFromParent();
    }
    if (TradeMainWidgetInstance && TradeMainWidgetInstance->IsInViewport())
    {
        TradeMainWidgetInstance->RemoveFromParent();
    }
}

void UPlayPCComponent::InitializeQuickSlotSystem()
{
    // ⭐⭐ [오류 C2065 해결] 'PC' 선언되지 않은 식별자 오류 해결 ⭐⭐
    APlayerController* PC = GetPlayerController();

    // PlayerStateRef, PlayWidgetInstance, MyASC가 UPlayPCComponent의 멤버 변수라고 가정합니다.
    if (!PC || !PlayerStateRef || !PlayWidgetInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayPCComponent] QuickSlot 초기화 실패: PC/PS 또는 PlayWidgetInstance가 유효하지 않습니다."));
        return;
    }

    UQuickSlotBarWidget* QuickSlotBar = PlayWidgetInstance->GetQuickSlotBarWidget();

    if (QuickSlotBar && QuickSlotManagerViewModel && MyASC.Get()) // MyASC는 TObjectPtr이므로 Get()으로 포인터를 가져와서 체크합니다.
    {
        // ⭐⭐ [오류 C2660 해결] ViewModel 호출 시 2개 인수로 통일 ⭐⭐
        QuickSlotManagerViewModel->InitializeViewModel(PlayerStateRef, MyASC.Get());

        // ⭐⭐ [오류 C2039 해결] SetQuickSlotManagerViewModel 함수 호출 ⭐⭐
        // 이 함수 선언은 UQuickSlotBarWidget.h에 추가되어야 합니다.
        QuickSlotBar->SetQuickSlotManagerViewModel(QuickSlotManagerViewModel);
        QuickSlotBarWidgetInstance = QuickSlotBar;
    }
}


void UPlayPCComponent::DelayedOpenHUDUI()
{
    UE_LOG(LogTemp, Warning, TEXT("[PlayPCC] DelayedOpenHUDUI() start."));
    APlayerController* PC = GetPlayerController();

    // 1. 최종 유효성 검사
    if (!PC || !PC->IsLocalController() || !PlayWidgetClass || !PlayerStateRef || !GameStateRef || !MyASC.Get())
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayPCC] OpenHUDUI Failed: Essential references are missing during final check."));
        return;
    }

    // 2. ViewModel 초기화 (AS가 준비되었으므로 성공적으로 초기화됨)
    PlayerStatusViewModel->InitializeViewModel(PlayerStateRef, MyASC.Get());
    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] PlayerStatusViewModel initialized."));

    GameStatusViewModel->InitializeViewModel(GameStateRef, MyASC.Get());
    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] GameStatusViewModel initialized."));

    PartyManagerViewModel->InitializeViewModel(PlayerStateRef, GameStateRef);
    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] PartyManagerViewModel Initialized in DelayedOpenHUDUI."));


    // 3. PlayWidgetInstance 생성
    if (!PlayWidgetInstance)
    {
        PlayWidgetInstance = CreateWidget<UPlayWidget>(PC, PlayWidgetClass);
    }

    // ⭐ PlayWidgetInstance 유효성 검증
    if (!PlayWidgetInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayPCC] PlayWidgetInstance FAILED TO CREATE. Aborting UI Init."));
        return;
    }

    // 4. QuickSlot 초기화 (PlayWidgetInstance 유효성 보장 후 호출)
    InitializeQuickSlotSystem();


    // 5. ViewModel 주입 및 화면 표시
    PlayWidgetInstance->SetPlayerStatusViewModel(PlayerStatusViewModel);
    PlayWidgetInstance->SetGameStatusViewModel(GameStatusViewModel);
    PlayWidgetInstance->SetPartyManagerViewModel(PartyManagerViewModel);

    UE_LOG(LogTemp, Log, TEXT("[PlayPCC] All ViewModels Injected into PlayWidget."));

    // 파티 리스트 뷰 바인딩 및 초기화 로직이 PlayWidget 내부에서 호출되는 함수라고 가정
    PlayWidgetInstance->SetsPartyListView();


    if (!PlayWidgetInstance->IsInViewport())
    {
        PlayWidgetInstance->AddToViewport();
        UE_LOG(LogTemp, Log, TEXT("[PlayPCC] PlayWidget added to viewport."));
    }
}

void UPlayPCComponent::HandlePhaseChanged(ETTTGamePhase NewPhase)
{
    // GameState 델리게이트 핸들러 (필요 시 구현)
}

void UPlayPCComponent::HandleRemainingTimeChanged(int32 NewRemainingTime)
{
    // GameState 델리게이트 핸들러 (필요 시 구현)
}

void UPlayPCComponent::TestFunction2()
{
    //UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    
    UE_LOG(LogTemp, Warning, TEXT("cccASC: %p"), MyASC.Get());
    if (!MyASC)
    {
        UE_LOG(LogTemp, Error, TEXT("[TestFunction2] Failed to get ASC. Cannot apply damage GE."));
        return;
    }

    // 2. GE 클래스가 에디터에서 설정되었는지 확인
    if (!DebugDamageGEClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[TestFunction2] DebugDamageGEClass is not set in editor. Cannot apply damage."));
        return;
    }
    APlayerController* PC = GetPlayerController();
    if (!PC->IsLocalController())
    {
        // 서버에서 실행
        
    }
    ServerApplyDamageGE();
    
    //// 3. GE 적용을 위한 Spec Handle 생성
    //FGameplayEffectContextHandle Context = MyASC->MakeEffectContext();
    //FGameplayEffectSpecHandle SpecHandle = MyASC->MakeOutgoingSpec(
    //    DebugDamageGEClass,
    //    1.0f, // 레벨은 1.0f (필요에 따라 변경)
    //    Context
    //);

    //if (SpecHandle.IsValid())
    //{
    //    // 4. GE 실행
    //    MyASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    //    UE_LOG(LogTemp, Warning,
    //        TEXT("[TestFunction2] Health Damage GE Executed."));
    //}

}


void UPlayPCComponent::ServerApplyDamageGE_Implementation()
{
    if (!MyASC || !DebugDamageGEClass) return;

    // 1. Context 생성
    FGameplayEffectContextHandle Context = MyASC->MakeEffectContext();

    // 2. Spec 생성
    FGameplayEffectSpecHandle SpecHandle = MyASC->MakeOutgoingSpec(DebugDamageGEClass, 1.f, Context);

    if (SpecHandle.IsValid())
    {
        // 3. 서버에서 GE 적용
        MyASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        UE_LOG(LogTemp, Warning, TEXT("[ServerApplyDamageGE] Health Damage GE Applied on Server."));
    }
}

//void UPlayPCComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//    //// ⭐ 모든 타이머를 해제합니다.
//    //FTimerManager& TimerManager = GetWorld()->GetTimerManager();
//
//    //TimerManager.ClearTimer(TestTimerHandle);
//    //TimerManager.ClearTimer(SetASCTimerHandle);
//    //TimerManager.ClearTimer(OpenReadyTimerHandle); // 특히 이 타이머를 반드시 해제해야 합니다.
//    //TimerManager.ClearTimer(RefreshTimerHandle);
//    //TimerManager.ClearTimer(TestTimerHandle2);
//
//    //// Super::EndPlay가 마지막에 호출되도록 합니다.
//    //Super::EndPlay(EndPlayReason);
//}
//
