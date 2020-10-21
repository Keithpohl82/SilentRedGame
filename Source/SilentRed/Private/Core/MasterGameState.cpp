// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/MasterGameState.h"
#include "SilentRed/Public/Core/MasterGameMode.h"
#include "SilentRed/Public/Core/BaseGameInstance.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "Net/UnrealNetwork.h"


AMasterGameState::AMasterGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumTeams = 0;
	RemainingTime = 0;
	bTimerPaused = false;
}



void AMasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMasterGameState, NumTeams);
	DOREPLIFETIME(AMasterGameState, RemainingTime);
	DOREPLIFETIME(AMasterGameState, TeamScores);
	DOREPLIFETIME(AMasterGameState, bTimerPaused);
	DOREPLIFETIME(AMasterGameState, NumRedPlayers);
	DOREPLIFETIME(AMasterGameState, NumBluePlayers);
}



void AMasterGameState::RequestFinishAndExitToMainMenu()
{
	if (AuthorityGameMode)
	{
		// we are server, tell the gamemode
		AMasterGameMode* const GameMode = Cast<AMasterGameMode>(AuthorityGameMode);
		if (GameMode)
		{
			GameMode->RequestFinishAndExitToMainMenu();
		}
	}
	else
	{
		// we are client, handle our own business
		UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->RemoveSplitScreenPlayers();
		}

		AMasterPlayerController* const PrimaryPC = Cast<AMasterPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
		if (PrimaryPC)
		{
			PrimaryPC->HandleReturnToMainMenu();
		}
	}
}
