// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/GameMode.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "MasterGameMode.generated.h"

//Rules of the Game. NOT REPLICATED. SERVER ONLY.
// Do we have Teams or does everyone play for their own score?
// What are the winning conditions ? How many kills does someone / a team need ?
// How are the points achieved ? Killing someone ? Stealing a flag ?
//What Characters will be used ? What weapons are allowed ? Only pistols ?

class APlayerStart;
class ABasePlayerState;
class FUniqueNetId;

/**
 * 
 */
UCLASS(config=Game)
class AMasterGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
	

	virtual void PreInitializeComponents() override;

	/** update remaining time */
	virtual void DefaultTimer();

	/** Initialize the game. This is called before actors' PreInitializeComponents. */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string. */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** starts match warmup */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Tries to spawn the player's pawn */
	virtual void RestartPlayer(AController* NewPlayer) override;

	/** called before startmatch */
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;
	
	/** always pick new random spawn */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;


protected:


	UPROPERTY(config)
	int32 TimeBetweenMatches;

	UPROPERTY(Config)
	int8 MaxPlayers;
	
	UPROPERTY(Config)
	int32 WarmupTime;

	//in Seconds 
	UPROPERTY(Config)
	int32 RoundTime;

	UPROPERTY(Config, EditDefaultsOnly)
	int32 KillScore;

	UPROPERTY(Config, EditDefaultsOnly)
	int32 DeathScore;

	UPROPERTY(Config)
	int32 NumberOfTeams;


	/** Handle for efficient management of DefaultTimer timer */
	FTimerHandle TimerHandle_DefaultTimer;

	/** check if player can use spawnpoint */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** check who won */
	virtual void DetermineMatchWinner();

	/** check if PlayerState is a winner */
	virtual bool IsWinner(ABasePlayerState* PlayerState) const;

	/** check if player should use spawnpoint */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;


public:
	
	/** finish current match and lock players */
	UFUNCTION(exec)
	void FinishMatch();

	/*Finishes the match and bumps everyone to main menu.*/
	/*Only GameInstance should call this function */
	void RequestFinishAndExitToMainMenu();
	

	int8 TeamColor;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	class USoundBase* CaptureSound;


};
