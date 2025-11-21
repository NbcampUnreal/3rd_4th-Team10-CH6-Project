#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/Base/BaseCharacter.h"
#include "PriestCharacter.generated.h"

class UAS_PriestAttributeSet;

UCLASS()
class TENTENTOWN_API APriestCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APriestCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {return IsValid(ASC)?ASC:nullptr;}

	virtual void RecalcStatsFromLevel(float NewLevel) override;
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> SkillAAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> SkillBAction;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Inputs")
	TObjectPtr<UInputAction> UltAction;
	
	UPROPERTY()
	const UAS_PriestAttributeSet* PriestAS;
};
