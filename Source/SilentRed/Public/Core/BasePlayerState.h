// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

//Replicated to everyone
//Player Name
//Player Score
//Player Ping
//Player Unique ID (if any)




/**
 * 
 */
UCLASS()
class SILENTRED_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:


	ABasePlayerState(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetTeamNum(int32 NewTeamNumber);

	UFUNCTION()
	void OnRep_SetTeam(int32 NewNumber);

	void SetTeamSkin();

	UPROPERTY(BlueprintReadWrite, Category = PlayerInfo)
	int PlayerPing;

	UPROPERTY(BlueprintReadWrite, Category = PlayerInfo)
	int32 GamePlayerID;

	UPROPERTY(BlueprintReadWrite, Category = PlayerInfo)
	FString PlayersName;

	UPROPERTY(BlueprintReadOnly, Category = Weapon)
	int32 NumShotsFired;


protected:



public:
	
	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 TeamNumber;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	float PlayerScore;

	/**Number of kills */
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	/** number of deaths */
	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float PlayerKDA;

	UFUNCTION()
	int32 GetTeamNumber() const;

	void ShotFired();



};
