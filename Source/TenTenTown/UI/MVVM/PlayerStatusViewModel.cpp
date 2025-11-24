#include "UI/MVVM/PlayerStatusViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/GAS/AS/MageAttributeSet/AS_MageAttributeSet.h"

UPlayerStatusViewModel::UPlayerStatusViewModel()
{
	// 생성자에서는 초기화만 진행
}

// ----------------------------------------------------------------------
// 초기화 및 정리
// ----------------------------------------------------------------------

void UPlayerStatusViewModel::InitializeViewModel(ATTTPlayerState* PlayerState)
{
    CachedPlayerState = PlayerState;
    if (!CachedPlayerState) return;

    UAbilitySystemComponent* ASC = CachedPlayerState->GetAbilitySystemComponent();
    if (!ASC) return;

    // --- 1. 공통 속성 (Health, Level) 초기화 ---
    // UAS_CharacterBase가 없으면 여기서 리턴하는 것은 적절합니다.
    const UAS_CharacterBase* BaseAS = ASC->GetSet<UAS_CharacterBase>();
    if (!BaseAS)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayerStatusVM] Failed to find UAS_CharacterBase on ASC. Aborting VM Initialize."));
        return;
    }

    float InitialMaxHealth = ASC->GetNumericAttributeBase(UAS_CharacterBase::GetMaxHealthAttribute());

    // MaxHealth가 유효할 때만 초기값 설정
    if (InitialMaxHealth > 0.0f)
    {
        MaxHealth = InitialMaxHealth;
        CurrentHealth = ASC->GetNumericAttributeBase(UAS_CharacterBase::GetHealthAttribute());
        SetLevel(FMath::RoundToInt(ASC->GetNumericAttributeBase(UAS_CharacterBase::GetLevelAttribute())));
        RecalculateHealthPercentage();
    }
    // else: MaxHealth가 0이면 구독 로직을 통해 OnMaxHealthChanged에서 초기화되도록 위임합니다.


    // --- 2. 특수 속성 (Mana) 초기화 및 구독 (조건부!) ---
    //여기서 캐릭터 클래스를 확인해야 합니다.
    // (선택된 캐릭터 클래스가 마법사인지 확인하는 로직이 필요합니다. 
    // TTTPlayerState에 이 정보가 있다면 사용해야 합니다.)

    // ATTTPlayerState::SelectedCharacterClass를 사용하여 마법사 클래스인지 확인한다고 가정
    // (예: IsMageClass(CachedPlayerState->SelectedCharacterClass) 같은 헬퍼 함수가 있다고 가정)
    // 현재는 코드가 없으므로, 일단 UAS_MageAttributeSet의 존재 여부로 대체합니다.

    const UAS_MageAttributeSet* MageAS = ASC->GetSet<UAS_MageAttributeSet>();

    if (MageAS) //마법사 Attribute Set이 등록되어 있을 때만 Mana 로직 실행
    {
        MaxMana = ASC->GetNumericAttributeBase(UAS_MageAttributeSet::GetMaxManaAttribute());
        CurrentMana = ASC->GetNumericAttributeBase(UAS_MageAttributeSet::GetManaAttribute());
        RecalculateManaPercentage();

        // 3. Mana 구독 설정 (마법사일 때만)
        ASC->GetGameplayAttributeValueChangeDelegate(UAS_MageAttributeSet::GetManaAttribute())
            .AddUObject(this, &UPlayerStatusViewModel::OnManaChanged);
        ASC->GetGameplayAttributeValueChangeDelegate(UAS_MageAttributeSet::GetMaxManaAttribute())
            .AddUObject(this, &UPlayerStatusViewModel::OnMaxManaChanged);

        //중요: SetManaUIVisibility(ESlateVisibility::Visible) 등 UI 활성화 로직 추가
    }
    else
    {
        //중요: 마법사가 아니면 Mana를 0으로 설정하고 UI를 비활성화 (숨김)
        MaxMana = 0.0f;
        CurrentMana = 0.0f;
        // SetManaUIVisibility(ESlateVisibility::Collapsed); 등 UI 비활성화 로직 추가
    }

    // --- 4. 공통 Attribute 변화 구독 설정 (Health, Level) ---
    // 이 로직은 MageAS/BaseAS 유효성 검사 후에 실행되어야 안전합니다.

    // 1. Level 구독
    ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetLevelAttribute())
        .AddUObject(this, &UPlayerStatusViewModel::OnLevelChanged);

    // 2. Health 구독
    ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute())
        .AddUObject(this, &UPlayerStatusViewModel::OnHealthChanged);
    ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute())
        .AddUObject(this, &UPlayerStatusViewModel::OnMaxHealthChanged);
}

void UPlayerStatusViewModel::CleanupViewModel()
{
    if (CachedPlayerState)
    {
        if (UAbilitySystemComponent* ASC = CachedPlayerState->GetAbilitySystemComponent())
        {
            // --- 공통 속성 해제 ---
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetLevelAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute()).RemoveAll(this);

            // --- 마법사 속성 조건부 해제 ---
            // Mana 구독은 마법사일 때만 설정되었으므로, 해제도 MageAS를 통해 진행
            if (ASC->GetSet<UAS_MageAttributeSet>())
            {
                ASC->GetGameplayAttributeValueChangeDelegate(UAS_MageAttributeSet::GetManaAttribute()).RemoveAll(this);
                ASC->GetGameplayAttributeValueChangeDelegate(UAS_MageAttributeSet::GetMaxManaAttribute()).RemoveAll(this);
            }            
        }
    }
    CachedPlayerState = nullptr;
    // Super::CleanupViewModel(); // UBaseViewModel에 있다면 호출
}

// ----------------------------------------------------------------------
// GAS 콜백 함수 구현
// ----------------------------------------------------------------------

void UPlayerStatusViewModel::OnLevelChanged(const FOnAttributeChangeData& Data)
{
	SetLevel(FMath::RoundToInt(Data.NewValue));
}

void UPlayerStatusViewModel::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	RecalculateHealthPercentage();
}

void UPlayerStatusViewModel::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue;
	RecalculateHealthPercentage();
}

void UPlayerStatusViewModel::OnManaChanged(const FOnAttributeChangeData& Data)
{
	CurrentMana = Data.NewValue;
	RecalculateManaPercentage();
}

void UPlayerStatusViewModel::OnMaxManaChanged(const FOnAttributeChangeData& Data)
{
	MaxMana = Data.NewValue;
	RecalculateManaPercentage();
}

// ----------------------------------------------------------------------
// 백분율 계산 로직 (ViewModel의 핵심 책임)
// ----------------------------------------------------------------------

void UPlayerStatusViewModel::RecalculateHealthPercentage()
{
	UE_LOG(LogTemp, Log, TEXT("Recalculating Health Percentage: CurrentHealth=%.2f, MaxHealth=%.2f"), CurrentHealth, MaxHealth);
	// CurrentHealth / MaxHealth 계산
	float NewPercentage = (MaxHealth > 0.0f) ? (CurrentHealth / MaxHealth) : 0.0f;

	UE_LOG(LogTemp, Log, TEXT("New Health Percentage: %.4f"), NewPercentage);
	SetHealthPercentage(FMath::Clamp(NewPercentage, 0.0f, 1.0f));
}

void UPlayerStatusViewModel::RecalculateManaPercentage()
{
	// CurrentMana / MaxMana 계산 후 StaminaPercentage Setter 사용
	float NewPercentage = (MaxMana > 0.0f) ? (CurrentMana / MaxMana) : 0.0f;
	SetStaminaPercentage(FMath::Clamp(NewPercentage, 0.0f, 1.0f));
}

// ----------------------------------------------------------------------
// UPROPERTY Setter 구현 (FieldNotify 브로드캐스트)
// ----------------------------------------------------------------------

void UPlayerStatusViewModel::SetLevel(int32 NewLevel)
{
	if (Level != NewLevel)
	{
		Level = NewLevel;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Level);
	}
}

void UPlayerStatusViewModel::SetExpPercentage(float NewPercentage)
{
	if (!FMath::IsNearlyEqual(ExpPercentage, NewPercentage))
	{
		ExpPercentage = NewPercentage;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ExpPercentage);
	}
}

void UPlayerStatusViewModel::SetHealthPercentage(float NewPercentage)
{
	if (!FMath::IsNearlyEqual(HealthPercentage, NewPercentage))
	{
		HealthPercentage = NewPercentage;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HealthPercentage);
	}
}

void UPlayerStatusViewModel::SetStaminaPercentage(float NewPercentage)
{
	if (!FMath::IsNearlyEqual(StaminaPercentage, NewPercentage))
	{
		StaminaPercentage = NewPercentage;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(StaminaPercentage);
	}
}