#include "UI/PCC/PCCBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"


UPCCBase::UPCCBase()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UPCCBase::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* UPCCBase::GetAbilitySystemComponent() const
{
	UE_LOG(LogTemp, Warning, TEXT("UPCCBase::GetAbilitySystemComponent called"));
    const APlayerController* PC = Cast<APlayerController>(GetOwner());

    if (PC)
    {
		UE_LOG(LogTemp, Warning, TEXT("PlayerController found in UPCCBase"));
        if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
        {
			UE_LOG(LogTemp, Warning, TEXT("PlayerState found in UPCCBase"));
            IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(PS);

            if (ASCInterface)
            {
				UE_LOG(LogTemp, Warning, TEXT("AbilitySystemInterface found in UPCCBase")); 
                return ASCInterface->GetAbilitySystemComponent();
            }
        }
    }
    return nullptr;
}

void UPCCBase::OnModeTagChanged(const FGameplayTag Tag, int32 NewCount)
{
}


