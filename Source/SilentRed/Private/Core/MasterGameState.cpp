// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/MasterGameState.h"
#include "Net/UnrealNetwork.h"


AMasterGameState::AMasterGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	NumTeams = 0;
	NumBluePlayers = 0;
	NumRedPlayers = 0;
	BluePoints = 0;
	RedPoints = 0;
}


int32 AMasterGameState::GetBluePoints()
{
	return BluePoints;
}

int32 AMasterGameState::GetRedPoints()
{
	return RedPoints;
}

void AMasterGameState::BeginPlay()
{
	Super::BeginPlay();
	GameSeconds = 0;
	GameMinutes = 20;
	SetGameTime();
}

void AMasterGameState::SetTimerSeconds()
{
	GameSeconds--;

	if (GameSeconds <= 0)
	{
		SetTimerMinuets();
		GameSeconds = 59;
	}
}

void AMasterGameState::SetTimerMinuets()
{
	GameMinutes--;
}

void AMasterGameState::SetGameTime()
{
	GetWorldTimerManager().SetTimer(TimerHandle_GameTimer, this, &AMasterGameState::SetTimerSeconds, 1.0f, true, 1.0f);
}

float AMasterGameState::GetGameSeconds()
{
	return GameSeconds;
}

float AMasterGameState::GetGameMinuets()
{
	return GameMinutes;
}

void AMasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMasterGameState, NumTeams);

	DOREPLIFETIME(AMasterGameState, TeamScores);

	DOREPLIFETIME(AMasterGameState, RedPoints);

	DOREPLIFETIME(AMasterGameState, BluePoints);

	DOREPLIFETIME(AMasterGameState, GameMinutes);

	DOREPLIFETIME(AMasterGameState, GameSeconds);

	DOREPLIFETIME(AMasterGameState, NumRedPlayers);

	DOREPLIFETIME(AMasterGameState, NumBluePlayers);
}
