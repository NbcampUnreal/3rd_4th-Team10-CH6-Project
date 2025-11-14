// TTTGameStateBase.cpp


#include "GameSystem/GameMode/TTTGameStateBase.h"

#include "Net/UnrealNetwork.h"

ATTTGameStateBase::ATTTGameStateBase()
{
	bReplicates = true;
	Wave=0;
}

void ATTTGameStateBase::OnRep_Phase()
{
	OnPhaseChanged.Broadcast(Phase);
}

void ATTTGameStateBase::OnRep_RemainingTime()
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);
}

void ATTTGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTTGameStateBase, Phase);
	DOREPLIFETIME(ATTTGameStateBase, RemainingTime);
	DOREPLIFETIME(ATTTGameStateBase, Wave);
}
