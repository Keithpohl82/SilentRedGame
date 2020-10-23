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
class ABasePlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	/** player killed someone */
	//void ScoreKill(ABasePlayerState* Victim, int32 Points);

	/** player died */
	//void ScoreDeath(ABasePlayerState* KilledBy, int32 Points);

	/** Sends kill (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutKill(class ABasePlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ABasePlayerState* KilledPlayerState);

	/** broadcast death to local clients */
	//UFUNCTION(Reliable, NetMulticast)
	//void BroadcastDeath(class ABasePlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ABasePlayerState* KilledPlayerState);


public:


	

	UFUNCTION()
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

	UFUNCTION(BlueprintCallable)
	int32 GetTeamNumber() const;

	void ShotFired();



};
