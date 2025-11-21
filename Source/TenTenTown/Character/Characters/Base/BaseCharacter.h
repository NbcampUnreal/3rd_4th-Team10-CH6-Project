#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UAS_CharacterBase;
class UAttributeSet;
class UInteractionSystemComponent;
struct FInputActionInstance;
class UCameraComponent;
class USpringArmComponent;
class ATTTPlayerState;
class UInputAction;
class UInputMappingContext;
class UGameplayAbility;
enum class ENumInputID : uint8;
class UAbilitySystemComponent;
class UCurveTable;

UCLASS()
class TENTENTOWN_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return IsValid(ASC)?ASC:nullptr;}
	
	void OnLevelChanged(const FOnAttributeChangeData& Data);
	virtual void RecalcStatsFromLevel(float NewLevel);
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void GiveDefaultAbility();

	UFUNCTION()
	void OnLevelUpInput(const FInputActionInstance& InputActionInstance);
	UFUNCTION(Server, Reliable)
	void Server_LevelUp();
	
	//인풋 액션
	//이동 및 공격
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputMappingContext> IMC;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> DashAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> AttackAction;

	//타워 설치
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> InstallAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> ConfirmAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> CancelAction;

	//디버깅용 레벨업
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> LevelUpAction;
	
	//IA 바인딩 함수
	void Move(const FInputActionInstance& FInputActionInstance);
	void Look(const FInputActionInstance& FInputActionInstance);
	virtual void ActivateGAByInputID(const FInputActionInstance& FInputActionInstance,ENumInputID InputID);
	void ConfirmInstall(const FInputActionInstance& FInputActionInstance);
	void CancelInstall(const FInputActionInstance& FInputActionInstance);
	
	//InputID, GA
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS|EnputIDGAMap")
	TMap <ENumInputID,TSubclassOf<UGameplayAbility>> InputIDGAMap;
	UPROPERTY(EditAnywhere, Category="GAS|Passive")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
	
	//기본 컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic Components")
	TObjectPtr<UInteractionSystemComponent> ISC;
	
	//주요 캐싱
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PlayerState")
	TObjectPtr<ATTTPlayerState> PS;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GAS|ASC")
	TObjectPtr<UAbilitySystemComponent> ASC = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="LevelUp")
	TObjectPtr<UCurveTable> LevelUpCurveTable;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GAS|Attributeset")
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;
	UPROPERTY()
	const UAS_CharacterBase* CharacterBaseAS;
};
