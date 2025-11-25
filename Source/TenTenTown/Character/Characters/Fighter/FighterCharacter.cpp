// Fill out your copyright notice in the Description page of Project Settings.


#include "FighterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/ENumInputID.h"
#include "TTTGameplayTags.h" 

AFighterCharacter::AFighterCharacter()
{
    JumpMaxCount = 2;
    GetCharacterMovement()->MaxWalkSpeed = 300.f;
    PrimaryActorTick.bCanEverTick = true;
}

void AFighterCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->ConsoleCommand("ShowDebug AbilitySystem 1");
        PC->ConsoleCommand("AbilitySystem.DebugAttribute Health MaxHealth");
    }
}

void AFighterCharacter::PossessedBy(AController* NewController)
{
    // 부모 클래스가 ASC 설정, AttributeSet 생성(배열 기반), 기본 Ability 지급을 처리
    Super::PossessedBy(NewController);

    if (ASC)
    {
        ASC->ApplyGameplayEffectToSelf(GE_FighterRegen->GetDefaultObject<UGameplayEffect>(),1.f,ASC->MakeEffectContext());
    }
}

void AFighterCharacter::OnRep_PlayerState()
{
    // 부모 클래스가 ASC, PS, ActorInfo 초기화를 처리
    Super::OnRep_PlayerState();
    
}

void AFighterCharacter::Tick(float DeltaTime)
{
    
}

void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // 부모 클래스가 Move, Look, Jump, Dash(Base) 등을 바인딩
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC) return;

    // --- Fighter 고유 액션 바인딩 ---
    // 중요: 부모의 protected 함수 접근을 위해 &ThisClass:: 사용

    // [Sprint]
    EIC->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::ActivateGAByInputID, ENumInputID::Sprint);
    EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::Sprint);

    // [Attack] - Base의 AttackAction과 별도로 Fighter 전용 NormalAttack 사용 시
    EIC->BindAction(NormalAttackAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::NormalAttack);
    EIC->BindAction(NormalAttackAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::UltimateNormalAttack);

    // [Right Charge]
    EIC->BindAction(RightChargeAttack, ETriggerEvent::Triggered, this, &ThisClass::ActivateGAByInputID, ENumInputID::RightChargeAttack);
    EIC->BindAction(RightChargeAttack, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::RightChargeAttack);
    
    // [Skills]
    EIC->BindAction(Ultimate,   ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
    EIC->BindAction(KickAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillA);
    
    EIC->BindAction(WhirlwindAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillB);
    EIC->BindAction(WhirlwindAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillB);
    
}

