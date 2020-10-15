// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"
#include "Net/UnrealNetwork.h"

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


void AMasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMasterPlayerController, TeamNum);
}


