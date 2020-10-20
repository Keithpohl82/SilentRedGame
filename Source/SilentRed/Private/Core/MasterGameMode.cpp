// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/MasterGameMode.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/Core/BaseGameInstance.h"
#include "SilentRed/Public/CTF_CapturePoint.h"
#include "SilentRed/Public/GamePlay/TeamsPlayerStart.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Characters/MasterSpectatorPawn.h"
#include "SilentRed/SilentRed.h"
#include "Kismet/GameplayStatics.h"


AMasterGameMode::AMasterGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Characters/BP_BaseCharacter"));
	DefaultPawnClass = PlayerPawnOb.Class;


	PlayerStateClass = ABasePlayerState::StaticClass();
	PlayerControllerClass = AMasterPlayerController::StaticClass();
	SpectatorClass = AMasterSpectatorPawn::StaticClass();
	GameStateClass = AMasterGameState::StaticClass();

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


void AMasterGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &AMasterGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void AMasterGameMode::DefaultTimer()
{
	// don't update timers for Play In Editor mode, it's not real match
	if (GetWorld()->IsPlayInEditor())
	{
		// start match if necessary.
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
		return;
	}
}

void AMasterGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	const UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance /*&& Cast<UBaseGameInstance>(GameInstance)->GetOnlineMode() != EOnlineMode::Offline*/)
	{
		bPauseable = false;
	}
}

void AMasterGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// GameSession can be NULL if the match is over
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void AMasterGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//Still move to add after game state setup

}

void AMasterGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

AActor* AMasterGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	
}

FString AMasterGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	ABasePlayerState* NewPlayerState = Cast<ABasePlayerState>(NewPlayerController->PlayerState);
	AMasterGameState* ThisGameState = GetGameState<AMasterGameState>();

	if (NewPlayerState)
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


bool AMasterGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	ATeamsPlayerStart* ShooterSpawnPoint = Cast<ATeamsPlayerStart>(SpawnPoint);
	

	return false;
}

void AMasterGameMode::DetermineMatchWinner()
{
	//Do nothing?
}

bool AMasterGameMode::IsWinner(ABasePlayerState* PlayerState) const
{
	return false;
}

