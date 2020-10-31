// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/CTFGameMode.h"
#include "SilentRed/Public/CTF_CapturePoint.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SilentRed/Public/GamePlay/TeamsPlayerStart.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"

ACTFGameMode::ACTFGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Characters/BP_TestPlayer"));
	DefaultPawnClass = PlayerPawnOb.Class;

	NumTeams = 2;
	bDelayedStart = true;

	RedTeam = 1;
	BlueTeam = 2;
	
}

void ACTFGameMode::InitGameState()
{
	Super::InitGameState();

	AMasterGameState* const MyGameState = Cast<AMasterGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->NumTeams = NumTeams;
	}
}

void ACTFGameMode::PostLogin(APlayerController* NewPlayer)
{
	// Place player on a team before Super (VoIP team based init, findplayerstart, etc)
	ABasePlayerState* NewPlayerState = CastChecked<ABasePlayerState>(NewPlayer->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);

	Super::PostLogin(NewPlayer);
}


int32 ACTFGameMode::ChooseTeam(ABasePlayerState* ForPlayerState) const
{
	int32 TeamNum;
	
	ABasePlayerState* PS = Cast<ABasePlayerState>(ForPlayerState);
	AMasterGameState* GState = GetGameState<AMasterGameState>();


	if (GState->NumRedPlayers <= GState->NumBluePlayers)
	{
		GState->NumRedPlayers++;
		return TeamNum = RedTeam;
		//PS->SetTeamNum(RedTeam);
	}
	else
	{
		GState->NumBluePlayers++;
		return TeamNum = BlueTeam;
		//PS->SetTeamNum(BlueTeam);
	}
}

void ACTFGameMode::CheckForWinner()
{
	AMasterGameState* CurrentGameState = Cast<AMasterGameState>(GameState);

	if (CurrentGameState->BluePoints == ScoreToWin)
	{
		FinishMatch();

	}
	if (CurrentGameState->RedPoints == ScoreToWin)
	{
		FinishMatch();
	}

}

void ACTFGameMode::DetermineMatchWinner()
{
	AMasterGameState const* const MyGameState = Cast<AMasterGameState>(GameState);
	int32 BestScore = MIN_uint32;
	int32 BestTeam = -1;
	int32 NumBestTeams = 1;

	for (int32 i = 0; i < MyGameState->TeamScores.Num(); i++)
	{
		const int32 TeamScore = MyGameState->TeamScores[i];
		if (BestScore < TeamScore)
		{
			BestScore = TeamScore;
			BestTeam = i;
			NumBestTeams = 1;
		}
		else if (BestScore == TeamScore)
		{
			NumBestTeams++;
		}
	}

	WinnerTeam = (NumBestTeams == 1) ? BestTeam : NumTeams;
}

bool ACTFGameMode::IsWinner(ABasePlayerState* PlayerState) const
{
	return PlayerState && PlayerState->GetTeamNumber() == WinnerTeam;
}

bool ACTFGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	if (Player)
	{
		ATeamsPlayerStart* TeamStart = Cast<ATeamsPlayerStart>(SpawnPoint);
		ABasePlayerState* PlayerState = Cast<ABasePlayerState>(Player->PlayerState);

		if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamNumber())
		{
			return false;
		}
	}

	return Super::IsSpawnpointAllowed(SpawnPoint, Player);
}

void ACTFGameMode::FlagCapture(int32 TeamThatCapturedIt)
{
	AMasterGameState* ThisGameState = GetGameState<AMasterGameState>();
	if (ThisGameState)
	{
		

		if (TeamThatCapturedIt == RedTeam)
		{	
				ACTF_CapturePoint* Loc = Cast<ACTF_CapturePoint>(GetWorld());
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CaptureSound, FVector::ZeroVector);
				
				ThisGameState->RedPoints++;
				CheckForWinner();
		}
		else if (TeamThatCapturedIt == BlueTeam)
		{
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CaptureSound, FVector::ZeroVector);

				ThisGameState->BluePoints++;
				CheckForWinner();
		}
	}
}
