#include "UI/MVVM/ResultSlotViewModel.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Character/Characters/Base/BaseCharacter.h"



void UResultSlotViewModel::InitializeSlot(ATTTPlayerController* InPlayerController, const FPlayerResultData& InPlayerResult)
{
	UE_LOG(LogTemp, Warning, TEXT("UResultSlotViewModel::InitializeSlot called"));
	CachedPlayerController = InPlayerController;

	if (!CachedPlayerController) { return; }

	UTTTGameInstance* TTTGI = CachedPlayerController->GetGameInstance<UTTTGameInstance>();
	PlayerName = InPlayerResult.PlayerName;
	KillCountText = FText::AsNumber(InPlayerResult.Kills);
	ScoreText = FText::AsNumber(InPlayerResult.Score);

	//CharacterIndexüũ
	UE_LOG(LogTemp, Warning, TEXT("CharacterIndex: %d"), InPlayerResult.CharacterIndex);
	const ABaseCharacter* BaseCharacterCDO = Cast<ABaseCharacter>(TTTGI->AvailableCharacterClasses[InPlayerResult.CharacterIndex]);
	if (BaseCharacterCDO)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseCharacterCDO found for index %d"), InPlayerResult.CharacterIndex);
		IconTexture = BaseCharacterCDO->CharacterIconTexture;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No BaseCharacterCDO found for index %d"), InPlayerResult.CharacterIndex);
		IconTexture = nullptr;
	}
	

	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerName);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(KillCountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ScoreText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
}

void UResultSlotViewModel::ReCharge()
{
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerName);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(KillCountText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ScoreText);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(IconTexture);
}
