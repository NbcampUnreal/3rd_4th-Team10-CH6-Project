#include "UI/MVVM/PartyStatusViewModel.h"
#include "Character/PS/TTTPlayerState.h"
#include "AbilitySystemInterface.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Engine/Texture2D.h"

UPartyStatusViewModel::UPartyStatusViewModel()
{
    
}

void UPartyStatusViewModel::InitializeViewModel()
{
}


void UPartyStatusViewModel::InitializeViewModel(ATTTPlayerState* PartyPlayerState)
{
    UE_LOG(LogTemp, Warning, TEXT("[VM INIT] PS=%s, ASI=%s"),
        *PartyPlayerState->GetPlayerName(),
        Cast<IAbilitySystemInterface>(PartyPlayerState) ? TEXT("YES") : TEXT("NO"));


    CachedPlayerState = PartyPlayerState;
    if (!CachedPlayerState) return;

    // 1. 초기 값 설정 (이름)
    SetNameText(FText::FromString(CachedPlayerState->GetPlayerName()));
    // HeadTexture는 PlayerState나 Character에서 가져와 SetHeadTexture(texture);를 호출해야 합니다.

    // 2. GAS 구독 시작
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            // Health 및 MaxHealth AttributeSet의 Delegate를 구독합니다.
            // AS_MageAttributeSet에 Health() 및 MaxHealth() Getter가 있다고 가정합니다.

            // Health 구독
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute())
                .AddUObject(this, &UPartyStatusViewModel::OnHealthChanged);

            // MaxHealth 구독
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute())
                .AddUObject(this, &UPartyStatusViewModel::OnMaxHealthChanged);

            // 초기 체력 비율 계산 및 설정
            /*const float CurrentHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetHealthAttribute());
            const float MaxHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetMaxHealthAttribute());

            if (MaxHealth > 0)
            {
                SetHealthPercentage(CurrentHealth / MaxHealth);
            }*/
            RecalculateHealthPercentage();
        }
    }

    //초상화 설정
    const APawn* PawnCDO = CachedPlayerState->SelectedCharacterClass->GetDefaultObject<APawn>();

    if (PawnCDO)
    {
        const ABaseCharacter* BaseCharacterCDO = Cast<ABaseCharacter>(PawnCDO);

        if (BaseCharacterCDO)
        {
            SetIconTexture(BaseCharacterCDO->CharacterIconTexture.Get());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[PlayerStatusVM] Failed to set Icon Texture: Pawn CDO is not ABaseCharacter."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[PlayerStatusVM] Failed to get Pawn CDO."));
    }
}

void UPartyStatusViewModel::CleanupViewModel()
{
    // GAS 구독 해제
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetHealthAttribute()).RemoveAll(this);
            ASC->GetGameplayAttributeValueChangeDelegate(UAS_CharacterBase::GetMaxHealthAttribute()).RemoveAll(this);
        }
    }

    CachedPlayerState = nullptr;
}


// -----------------------------------------------------
// GAS 콜백 함수 (데이터 수신 및 가공)
// -----------------------------------------------------

void UPartyStatusViewModel::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            const float CurrentMaxHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetMaxHealthAttribute());
            const float NewHealthPercentage = CurrentMaxHealth > 0 ? Data.NewValue / CurrentMaxHealth : 0.0f;
            SetHealthPercentage(NewHealthPercentage); // Setter 호출 -> UI 브로드캐스트
        }
    }
}

void UPartyStatusViewModel::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
    // MaxHealth가 변경되면 현재 체력 기준으로 비율을 다시 계산해야 합니다.
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            const float CurrentHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetHealthAttribute());
            const float NewMaxHealth = Data.NewValue;
            const float NewHealthPercentage = NewMaxHealth > 0 ? CurrentHealth / NewMaxHealth : 0.0f;
            SetHealthPercentage(NewHealthPercentage); // Setter 호출 -> UI 브로드캐스트
        }
    }
}


// -----------------------------------------------------
// UPROPERTY Setter 구현 (FieldNotify 브로드캐스트)
// -----------------------------------------------------
void UPartyStatusViewModel::SetIconTexture(UTexture2D* InTexture)
{
    if (IconTexture != InTexture)
    {
        IconTexture = InTexture;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
    }
}
void UPartyStatusViewModel::SetNameText(FText NewText)
{
    if (!NameText.EqualTo(NewText))
    {
        NameText = NewText;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(NameText);
    }
}

void UPartyStatusViewModel::SetHealthPercentage(float NewValue)
{
    HealthPercentage = NewValue;
    UE_LOG(LogTemp, Warning, TEXT("VIEW MODEL SET: HealthPct=%.2f for %s"), NewValue, *CachedPlayerState->GetPlayerName()); // ⭐ 추가
    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HealthPercentage);
    //if (HealthPercentage != NewValue)
    //{
    //    HealthPercentage = NewValue;
    //    UE_LOG(LogTemp, Warning, TEXT("VIEW MODEL SET: HealthPct=%.2f for %s"), NewValue, *CachedPlayerState->GetPlayerName()); // ⭐ 추가
    //    UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HealthPercentage);
    //}
}

void UPartyStatusViewModel::SetHeadTexture(UTexture2D* NewTexture)
{
    if (HeadTexture != NewTexture)
    {
        HeadTexture = NewTexture;
        UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(HeadTexture);
    }
}

void UPartyStatusViewModel::RecalculateHealthPercentage()
{
    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayerState))
    {
        if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            const float CurrentHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetHealthAttribute());
            const float MaxHealth = ASC->GetNumericAttribute(UAS_CharacterBase::GetMaxHealthAttribute());

            // [DEBUG: 현재 획득 값 로그 출력]
            UE_LOG(LogTemp, Warning, TEXT("aaaRecalculateHealthPercentage: Health=%.2f, MaxHealth=%.2f for %s"),
                CurrentHealth, MaxHealth, *CachedPlayerState->GetPlayerName());

            const float NewHealthPercentage = MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.0f;
            SetHealthPercentage(NewHealthPercentage);
        }
    }
}