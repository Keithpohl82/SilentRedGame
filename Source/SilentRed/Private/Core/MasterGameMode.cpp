// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/MasterGameMode.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/CTF_CapturePoint.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/SilentRed.h"
#include "Kismet/GameplayStatics.h"



void AMasterGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ABasePlayerState* PlayerState = NewPlayer->GetPlayerState<ABasePlayerState>();
	AMasterGameState* MyGameState = GetGameState<AMasterGameState>();
	APawn* PlayerPawn = NewPlayer->GetPawn();
	ABaseCharacter* PlayerChar = Cast<ABaseCharacter>(NewPlayer);
	AMasterPlayerController* Controller = Cast<AMasterPlayerController>(NewPlayer);

	if (NewPlayer)
	{
		ABasePlayerState* PS = Cast<ABasePlayerState>(NewPlayer->PlayerState);
		if (PS && GameState)
		{
			uint8 NumRedTeam = 0;
			uint8 NumBlueTeam = 0;
			
			if (NumRedTeam <= NumBlueTeam)
			{
				PS->SetTeamNum(RedTeam);
				NumRedTeam++;
			}
			else
			{
				PS->SetTeamNum(BlueTeam);
				NumBlueTeam++;
			}

		}

	}
	
}


void AMasterGameMode::FlagCapture(uint8 TeamThatCapturedIt)
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
