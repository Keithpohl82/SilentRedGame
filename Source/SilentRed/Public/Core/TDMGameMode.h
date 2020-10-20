// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Core/MasterGameMode.h"
#include "TDMGameMode.generated.h"

class ABasePlayerState;
class APlayerStart;

/**
 * 
 */
UCLASS(Config=Game)
class ATDMGameMode : public AMasterGameMode
{
	GENERATED_UCLASS_BODY()
	
	/** setup team changes at player login */
	void PostLogin(APlayerController* NewPlayer) override;

	/** initialize replicated game data */
	virtual void InitGameState() override;



protected:

	/** number of teams */
	int32 NumTeams;

	/** best team */
	int32 WinnerTeam;

	/** pick team with least players in or random when it's equal */
	int32 ChooseTeam(ABasePlayerState* ForPlayerState) const;

	/** check who won */
	virtual void DetermineMatchWinner() override;

	/** check if PlayerState is a winner */
	virtual bool IsWinner(ABasePlayerState* PlayerState) const override;

	/** check team constraints */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;
};
