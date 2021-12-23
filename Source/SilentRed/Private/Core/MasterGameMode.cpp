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
#include "GameFramework/GameSession.h"
#include "EngineUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "Components/CapsuleComponent.h"



AMasterGameMode::AMasterGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Characters/BP_TestPlayer"));
	DefaultPawnClass = PlayerPawnOb.Class;



	PlayerStateClass = ABasePlayerState::StaticClass();
	PlayerControllerClass = AMasterPlayerController::StaticClass();
	SpectatorClass = AMasterSpectatorPawn::StaticClass();
	GameStateClass = AMasterGameState::StaticClass();

	//RoundTime = 300;

	PlayerRespawnTime = 3;

}


void AMasterGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
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

	AMasterGameState* const MyGameState = Cast<AMasterGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
	{
		MyGameState->RemainingTime--;


		if (MyGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				RestartGame();
			}
			else if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();

				// Send end round events
				for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
				{
					AMasterPlayerController* PlayerController = Cast<AMasterPlayerController>(*It);

					if (PlayerController && MyGameState)
					{
						ABasePlayerState* PlayerState = Cast<ABasePlayerState>((*It)->PlayerState);
						const bool bIsWinner = IsWinner(PlayerState);

						//PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
					}
				}
			}
			else if (GetMatchState() == MatchState::WaitingToStart)
			{
				StartMatch();
				
			}
		}
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

void AMasterGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (bDelayedStart)
	{
		// start warmup if needed
		AMasterGameState* const MyGameState = Cast<AMasterGameState>(GameState);
		if (MyGameState && MyGameState->RemainingTime == 0)
		{
			const bool bWantsMatchWarmup = !GetWorld()->IsPlayInEditor();
			if (bWantsMatchWarmup && WarmupTime > 0)
			{
				MyGameState->RemainingTime = WarmupTime;
			}
			else
			{
				MyGameState->RemainingTime = 0.0f;
			}
		}
	}
}

void AMasterGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AMasterGameState* const MyGameState = Cast<AMasterGameState>(GameState);
	MyGameState->RemainingTime = RoundTime;

	// notify players
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AMasterPlayerController* PC = Cast<AMasterPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameStarted();
		}
	}
}

bool AMasterGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return true;
}

AActor* AMasterGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* TestSpawn = *It;
		if (TestSpawn->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			BestStart = TestSpawn;
			break;
		}
		else
		{
			if (IsSpawnpointAllowed(TestSpawn, Player))
			{
				if (IsSpawnpointPreferred(TestSpawn, Player))
				{
					PreferredSpawns.Add(TestSpawn);
				}
				else
				{
					FallbackSpawns.Add(TestSpawn);
				}
			}
		}
	}


	if (BestStart == NULL)
	{
		if (PreferredSpawns.Num() > 0)
		{
			BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
		}
		else if (FallbackSpawns.Num() > 0)
		{
			BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
		}
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
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

bool AMasterGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Cast<ACharacter>((*DefaultPawnClass)->GetDefaultObject<ACharacter>());


	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (ACharacter* OtherPawn : TActorRange<ACharacter>(GetWorld()))
		{
			if (OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->GetCapsuleComponent()->GetScaledCapsuleRadius() + OtherPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void AMasterGameMode::FinishMatch()
{
	AMasterGameState* const MyGameState = Cast<AMasterGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
		DetermineMatchWinner();

		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			ABasePlayerState* PlayerState = Cast<ABasePlayerState>((*It)->PlayerState);
			const bool bIsWinner = IsWinner(PlayerState);

			(*It)->GameHasEnded(NULL, bIsWinner);
		}

		// lock all pawns
		// pawns are not marked as keep for seamless travel, so we will create new pawns on the next match rather than
		// turning these back on.
		for (APawn* Pawn : TActorRange<APawn>(GetWorld()))
		{
			Pawn->TurnOff();
		}

		// set up to restart the match
		MyGameState->RemainingTime = TimeBetweenMatches;
	}
}

void AMasterGameMode::RestartDeadPlayer()
{
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AMasterGameMode::RespawnPlayer, PlayerRespawnTime, false);
}

void AMasterGameMode::RespawnPlayer()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

void AMasterGameMode::RequestFinishAndExitToMainMenu()
{
	FinishMatch();

	UBaseGameInstance* const GameInstance = Cast<UBaseGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->RemoveSplitScreenPlayers();
	}

	AMasterPlayerController* LocalPrimaryController = nullptr;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AMasterPlayerController* Controller = Cast<AMasterPlayerController>(*Iterator);

		if (Controller == NULL)
		{
			continue;
		}

		if (!Controller->IsLocalController())
		{
			const FText RemoteReturnReason = NSLOCTEXT("NetworkErrors", "HostHasLeft", "Host has left the game.");
			Controller->ClientReturnToMainMenuWithTextReason(RemoteReturnReason);
		}
		else
		{
			LocalPrimaryController = Controller;
		}
	}

	// GameInstance should be calling this from an EndState.  So call the PC function that performs cleanup, not the one that sets GI state.
	if (LocalPrimaryController != NULL)
	{
		LocalPrimaryController->HandleReturnToMainMenu();
	}
}

