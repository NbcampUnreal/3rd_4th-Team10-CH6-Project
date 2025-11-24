// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Fighter.h"

// Engine & Components
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"

// GAS & Attributes
#include "AbilitySystemComponent.h"
#include "Character/GAS/AS/FighterAttributeSet/AS_FighterAttributeSet.h"

// Project Specifics
#include "ENumInputID.h"
#include "TTTGameplayTags.h" // GASTAG 네임스페이스 (프로젝트 경로 확인 필요)

ACharacter_Fighter::ACharacter_Fighter()
{
    // ABaseCharacter 생성자에서 Camera, SpringArm, ISC 등은 이미 생성되었습니다.
    // 여기서는 Fighter만의 고유 수치만 설정합니다.

    // Fighter는 점프가 2단입니다.
    JumpMaxCount = 2;

    // 이동 속도 설정 (Base는 600, Fighter는 300)
    GetCharacterMovement()->MaxWalkSpeed = 300.f;

    // Tick 활성화 (HUD 디버깅용)
    PrimaryActorTick.bCanEverTick = true;
}

void ACharacter_Fighter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->ConsoleCommand("ShowDebug AbilitySystem 1");
        PC->ConsoleCommand("AbilitySystem.DebugAttribute Health MaxHealth");
    }
}

void ACharacter_Fighter::PossessedBy(AController* NewController)
{
    // 부모 클래스가 ASC 설정, AttributeSet 생성(배열 기반), 기본 Ability 지급을 처리합니다.
    Super::PossessedBy(NewController);

}

void ACharacter_Fighter::OnRep_PlayerState()
{
    // 부모 클래스가 ASC, PS, ActorInfo 초기화를 처리합니다.
    Super::OnRep_PlayerState();
    
}

void ACharacter_Fighter::Tick(float DeltaTime)
{
    
}

void ACharacter_Fighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // 부모 클래스가 Move, Look, Jump, Dash(Base) 등을 바인딩합니다.
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

    // [Install & Structure]
    EIC->BindAction(InstallAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::InstallStructure);
    EIC->BindAction(ConfirmAction, ETriggerEvent::Started, this, &ThisClass::ConfirmInstall);
    EIC->BindAction(CancelAction,  ETriggerEvent::Started, this, &ThisClass::CancelInstall);

    // [Skills]
    EIC->BindAction(Ultimate,   ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::Ult);
    EIC->BindAction(KickAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillA);
    
    EIC->BindAction(WhirlwindAction, ETriggerEvent::Started, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillB);
    EIC->BindAction(WhirlwindAction, ETriggerEvent::Completed, this, &ThisClass::ActivateGAByInputID, ENumInputID::SkillB);
}

void ACharacter_Fighter::ConfirmInstall(const FInputActionInstance& InputActionInstance)
{
    if (!ASC) return;

    // 설치 모드인지 확인 (GASTAG::State_IsInstall 태그 체크)
    if (ASC->HasMatchingGameplayTag(GASTAG::State_IsInstall))
    {
        FGameplayEventData Payload;
        ASC->HandleGameplayEvent(GASTAG::State_Structure_Confirm, &Payload);
    }
}

void ACharacter_Fighter::CancelInstall(const FInputActionInstance& InputActionInstance)
{
    if (!ASC) return;

    // 설치 모드인지 확인
    if (ASC->HasMatchingGameplayTag(GASTAG::State_IsInstall))
    {
        FGameplayEventData Payload;
        ASC->HandleGameplayEvent(GASTAG::State_Structure_Cancel, &Payload);
    }
}