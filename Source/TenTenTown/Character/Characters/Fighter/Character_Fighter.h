// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "Character_Fighter.generated.h"

class UAS_FighterAttributeSet;
class UInputAction;
struct FInputActionInstance;

/**
 * ABaseCharacter를 상속받아 Fighter 고유의 기능을 구현하는 클래스
 */
UCLASS()
class TENTENTOWN_API ACharacter_Fighter : public ABaseCharacter
{
    GENERATED_BODY()

public:
    ACharacter_Fighter();

protected:
    // --- Life Cycle & Overrides ---
    
    virtual void BeginPlay() override;
    
    // 매 프레임 호출 (Fighter 전용 디버그 HUD - 스태미나 포함)
    virtual void Tick(float DeltaTime) override;

    // 서버: 컨트롤러 빙의 시 호출 (FighterAttributeSet 캐싱)
    virtual void PossessedBy(AController* NewController) override;

    // 클라이언트: PlayerState 복제 시 호출 (FighterAttributeSet 캐싱)
    virtual void OnRep_PlayerState() override;

    // 입력 바인딩 설정 (Fighter 전용 키 바인딩)
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    // --- Fighter 전용 인풋 액션 (Base에 없는 것들) ---
    
    // Fighter 고유 일반 공격
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Fighter")
    TObjectPtr<UInputAction> NormalAttackAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Fighter")
    TObjectPtr<UInputAction> RightChargeAttack;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Fighter")
    TObjectPtr<UInputAction> KickAction;

    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs|Fighter")
    TObjectPtr<UInputAction> WhirlwindAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs|Fighter")
    TObjectPtr<UInputAction> Ultimate;
    

    // --- Fighter 전용 함수 ---
    
    void ConfirmInstall(const FInputActionInstance& InputActionInstance);
    void CancelInstall(const FInputActionInstance& InputActionInstance);


    // --- Fighter 전용 어트리뷰트 셋 캐싱 --
};