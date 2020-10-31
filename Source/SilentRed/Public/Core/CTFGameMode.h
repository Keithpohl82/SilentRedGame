// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/MasterGameMode.h"
#include "CTFGameMode.generated.h"

/**
 * 
 */
UCLASS(Config=Game)
class ACTFGameMode : public AMasterGameMode
{
	GENERATED_UCLASS_BODY()

	/** setup team changes at player login */
	void PostLogin(APlayerController* NewPlayer) override;

	/** initialize replicated game data */
	virtual void InitGameState() override;



protected:

	UPROPERTY(Config)
	int32 NumTeams;

	/** best team */
	int32 WinnerTeam;

	UPROPERTY(Config)
	int32 ScoreToWin;

	/** pick team with least players in or random when it's equal */
	int32 ChooseTeam(ABasePlayerState* ForPlayerState) const;

	void CheckForWinner();

	/** check who won */
	virtual void DetermineMatchWinner() override;

	/** check if PlayerState is a winner */
	virtual bool IsWinner(ABasePlayerState* PlayerState) const override;

	/** check team constraints */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;
	
	bool bATeamWon;

public:

	int8 RedTeam;
	int8 BlueTeam;
	
	void FlagCapture(int32 TeamThatCapturedIt);

};
