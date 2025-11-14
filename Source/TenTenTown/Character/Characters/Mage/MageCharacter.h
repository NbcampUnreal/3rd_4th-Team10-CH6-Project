//MageCharacter.cpp

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "MageCharacter.generated.h"

class UAttributeSet;
class UAS_MageAttributeSet;
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
class UStaticMeshComponent;

UCLASS()
class TENTENTOWN_API AMageCharacter : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMageCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return IsValid(ASC)?ASC:nullptr;}
	
	UPROPERTY(EditDefaultsOnly, Category="Mage|Weapon")
	FName WandAttachSocket = TEXT("WandAttach");
	

	//Getter 함수
	UFUNCTION(BlueprintPure, Category="Mage|Weapon")
	UStaticMeshComponent* GetWandMesh() const { return WandMesh; };
	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;

	void GiveDefaultAbility();
	UStaticMeshComponent* FindStaticMeshCompByName(FName Name) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Mage|Weapon")
	TObjectPtr<class UStaticMeshComponent> WandMesh;
	
public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	//인풋 액션
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputMappingContext> IMC;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> BlinkAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> FireballAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> FlameWallAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> FlameThrowerAction;
	
	//인풋 액션 바인딩 함수
	void Move(const FInputActionInstance& FInputActionInstance);
	void Look(const FInputActionInstance& FInputActionInstance);
	void ActivateGAByInputID(const FInputActionInstance& FInputActionInstance,ENumInputID InputID);

	//InputID, GA
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "GAS|EnputIDGAMap")
	TMap <ENumInputID,TSubclassOf<UGameplayAbility>> InputIDGAMap;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="GAS|Attributeset")
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;
	
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
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GAS|AS")
	const UAS_MageAttributeSet* MageAS;
};
