// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION(BlueprintCallable)
	int32 GetBluePoints();

	UFUNCTION(BlueprintCallable)
	int32 GetRedPoints();

	/** accumulated score per team */
	UPROPERTY(Transient, Replicated)
	TArray<int32> TeamScores;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 NumRedPlayers;
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 NumBluePlayers;

	UPROPERTY(Replicated, Transient)
	int32 NumTeams;

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RedPoints;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 BluePoints;

	FTimerHandle TimerHandle_GameTimer;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
	int32 GameSeconds;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
	int32 GameMinutes;

	void SetTimerSeconds();

	void SetTimerMinuets();


	void SetGameTime();

	float GetGameSeconds();

	float GetGameMinuets();
};
