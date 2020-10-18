// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/MasterGameMode.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/CTF_CapturePoint.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/SilentRed.h"
#include "Kismet/GameplayStatics.h"

AMasterGameMode::AMasterGameMode()
{
	RedTeam = 1;
	BlueTeam = 2;
}



void AMasterGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ABasePlayerState* PlayerState = NewPlayer->GetPlayerState<ABasePlayerState>();
	AMasterGameState* GState = GetGameState<AMasterGameState>();
	APawn* PlayerPawn = NewPlayer->GetPawn();
	ABaseCharacter* PlayerChar = Cast<ABaseCharacter>(NewPlayer);
	AMasterPlayerController* Controller = Cast<AMasterPlayerController>(NewPlayer);

	/*if (NewPlayer)
	{
		ABasePlayerState* PS = Cast<ABasePlayerState>(NewPlayer->PlayerState);
		if (PS && GState)
		{

			
			if (GState->NumRedPlayers <= GState->NumBluePlayers)
			{
				PS->SetTeamNum(RedTeam);
				GState->NumRedPlayers++;
			}
			else
			{
				PS->SetTeamNum(BlueTeam);
				GState->NumBluePlayers++;
			}

		}

	}*/
	
}


FString AMasterGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	ABasePlayerState* NewPlayerState = Cast<ABasePlayerState>(NewPlayerController->PlayerState);
	AMasterGameState* ThisGameState = GetGameState<AMasterGameState>();

	if (NewPlayerState )
	{
		if (ThisGameState->NumRedPlayers <= ThisGameState->NumBluePlayers)
		{
			
			NewPlayerState->SetTeamNum(1);
			ThisGameState->NumRedPlayers++;
		}
		else 
		{
			NewPlayerState->SetTeamNum(2);
			ThisGameState->NumBluePlayers++;
		}
	} 
	
	return Result;
}

void AMasterGameMode::FlagCapture(int32 TeamThatCapturedIt)
{
	AMasterGameState* ThisGameState = GetGameState<AMasterGameState>();
	if (ThisGameState)
	{
		if (TeamThatCapturedIt == RedTeam)
		{
			ACTF_CapturePoint* Loc = Cast<ACTF_CapturePoint>(GetWorld());

			ThisGameState->RedPoints++;
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CaptureSound, FVector::ZeroVector);
		}
		else if (TeamThatCapturedIt == BlueTeam)
		{
			ThisGameState->BluePoints++;
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CaptureSound, FVector::ZeroVector);
		}
	}
}
