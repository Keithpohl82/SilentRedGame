// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MasterGameState.generated.h"

//REPLICATED TO ALL CLIENTS.
//Tracks game informations such as team kills, flags captured, and other scoring.
//Elapsed time of the match.


/**
 * 
 */
UCLASS()
class SILENTRED_API AMasterGameState : public AGameStateBase
{
	GENERATED_BODY()
	
};
