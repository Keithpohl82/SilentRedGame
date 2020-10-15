// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MasterGameMode.generated.h"

//Rules of the Game. NOT REPLICATED. SERVER ONLY.
// Do we have Teams or does everyone play for their own score?
// What are the winning conditions ? How many kills does someone / a team need ?
// How are the points achieved ? Killing someone ? Stealing a flag ?
//What Characters will be used ? What weapons are allowed ? Only pistols ?

/**
 * 
 */
UCLASS(config=Game)
class SILENTRED_API AMasterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	


protected:

	UPROPERTY(Config)
	int32 MaxPlayers;
	
	UPROPERTY(Config)
	int32 WarmupTime;

	//in Seconds 
	UPROPERTY(Config)
	int32 RoundTime;

	UPROPERTY(Config, EditDefaultsOnly)
	int32 KillScore;

	UPROPERTY(Config, EditDefaultsOnly)
	int32 DeathScore;

public:
	

	int32 TeamColor;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	class USoundBase* CaptureSound;

	void FlagCapture(uint8 TeamThatCapturedIt);

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
