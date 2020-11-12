// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/GameState.h"
#include "MasterGameState.generated.h"

//REPLICATED TO ALL CLIENTS.
//Tracks game informations such as team kills, flags captured, and other scoring.
//Elapsed time of the match.


class ABaseCharacter;



/**
 * 
 */
UCLASS()
class AMasterGameState : public AGameState
{
	GENERATED_UCLASS_BODY()
	
		

public:


	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 RedPoints;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 BluePoints;

	int32 RedTeam = 1;
	int32 BlueTeam = 2;

	UPROPERTY(Replicated)
	int32 NumRedPlayers;

	UPROPERTY(Replicated)
	int32 NumBluePlayers;

	UPROPERTY(EditAnywhere)
	USoundBase* OneMinRemaining;

	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	UPROPERTY(Transient, Replicated)
	int32 NumTeams;

	/** accumulated score per team */
	UPROPERTY(Transient, Replicated)
	TArray<int32> TeamScores;

	/** time left for warmup / match */
	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;

	void PlayTimeLeft();

	void RequestFinishAndExitToMainMenu();


};
