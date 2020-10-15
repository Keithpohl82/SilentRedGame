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
			for (APlayerState* It : GameState->PlayerArray)
			{
				ABasePlayerState* OtherPS = Cast<ABasePlayerState>(It);
				if (OtherPS)
				{
					if (OtherPS->TeamColor == 1)
					{
						NumBlueTeam++;
					}
					else
					{
						NumRedTeam++;
					}
				}

			}
			if (NumRedTeam > NumBlueTeam)
			{
				PS->TeamColor = 2;
			}
			else
			{
				PS->TeamColor = 1;
			}

		}

	}
	
}


void AMasterGameMode::FlagCapture(uint8 TeamThatCapturedIt)
{
	AMasterGameState* ThisGameState = GetGameState<AMasterGameState>();
	if (ThisGameState)
	{
		if (TeamThatCapturedIt == 1)
		{
			ACTF_CapturePoint* Loc = Cast<ACTF_CapturePoint>(GetWorld());

			ThisGameState->RedPoints++;
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CaptureSound, FVector::ZeroVector);
		}
		else if (TeamThatCapturedIt == 2)
		{
			ThisGameState->BluePoints++;
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CaptureSound, FVector::ZeroVector);
		}
	}
}
