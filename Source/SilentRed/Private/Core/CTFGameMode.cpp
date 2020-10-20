// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/CTFGameMode.h"
#include "SilentRed/Public/CTF_CapturePoint.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SilentRed/Public/GamePlay/TeamsPlayerStart.h"
#include "SilentRed/Public/Core/BasePlayerState.h"

ACTFGameMode::ACTFGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Characters/BP_BaseCharacter"));
	DefaultPawnClass = PlayerPawnOb.Class;

	NumTeams = 2;
	bDelayedStart = true;

	

}


void ACTFGameMode::PostLogin(APlayerController* NewPlayer)
{
	// Place player on a team before Super (VoIP team based init, findplayerstart, etc)
	ABasePlayerState* NewPlayerState = CastChecked<ABasePlayerState>(NewPlayer->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);

	Super::PostLogin(NewPlayer);
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

int32 ACTFGameMode::ChooseTeam(ABasePlayerState* ForPlayerState) const
{
	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(NumTeams);

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ABasePlayerState const* const TestPlayerState = Cast<ABasePlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamNumber()))
		{
			TeamBalance[TestPlayerState->GetTeamNumber()]++;
		}
	}

	// find least populated one
	int32 BestTeamScore = TeamBalance[0];
	for (int32 i = 1; i < TeamBalance.Num(); i++)
	{
		if (BestTeamScore > TeamBalance[i])
		{
			BestTeamScore = TeamBalance[i];
		}
	}

	// there could be more than one...
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamBalance.Num(); i++)
	{
		if (TeamBalance[i] == BestTeamScore)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return RandomBestTeam;
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
