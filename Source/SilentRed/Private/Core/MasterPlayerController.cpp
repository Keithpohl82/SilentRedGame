// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "Online.h"
#include "Interfaces/OnlineEventsInterface.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"
#include "Net/UnrealNetwork.h"


AMasterPlayerController::AMasterPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bAllowGameActions = true;

	bGameEndedFrame = false;

	bHasSentStartEvents = false;

	Kills = 0;

	Deaths = 0;

	KDA = Deaths / Kills; 
}


void AMasterPlayerController::HandleReturnToMainMenu()
{
	// nothing for now
}

void AMasterPlayerController::RestartRecording()
{
	if (UWorld* World = GetWorld())
	{
		if (UDemoNetDriver* DemoDriver = World->DemoNetDriver)
		{
			DemoDriver->GotoTimeInSeconds(0.0f);
		}
	}
}




void AMasterPlayerController::OnKill()
{
	AMasterGameState* GS = Cast<AMasterGameState>(GetWorld()->GetGameState());
	
}



float AMasterPlayerController::GetKDA()
{
	return Kills / Deaths;
}

void AMasterPlayerController::ClientGameStarted_Implementation()
{
	bAllowGameActions = true;

	// Enable controls mode now the game has started
	SetIgnoreMoveInput(false);

	
	bGameEndedFrame = false;

	bHasSentStartEvents = true;
		
	
}



void AMasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMasterPlayerController, TeamNum);
	DOREPLIFETIME(AMasterPlayerController, Kills);
	DOREPLIFETIME(AMasterPlayerController, Deaths);
	DOREPLIFETIME(AMasterPlayerController, KDA);
	DOREPLIFETIME(AMasterPlayerController, Points);
}


