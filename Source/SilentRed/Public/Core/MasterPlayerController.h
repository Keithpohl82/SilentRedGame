// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/PlayerController.h"
#include "MasterPlayerController.generated.h"

//NOT REPLICATED players only know of their own.

/**
 * 
 */
UCLASS(Config=Game)
class AMasterPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()
	


public:

	/** Cleans up any resources necessary to return to main menu.  Does not modify GameInstance state. */
	virtual void HandleReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = Replays)
	void RestartRecording();

	void OnKill();

	UFUNCTION(BlueprintCallable)
	float GetKDA();

	TSharedPtr<const FUniqueNetId> UniqueNetID;

	UFUNCTION(BlueprintCallable, Category = stats)
	void SetTestStats();

protected:

	/** if set, gameplay related actions (movement, weapn usage, etc) are allowed */
	uint8 bAllowGameActions : 1;

	/** true for the first frame after the game has ended */
	uint8 bGameEndedFrame : 1;


public:

	UPROPERTY(Replicated)
	int32 TeamNum = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float Kills;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float Deaths;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float KDA;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float Points;

	/** notify player about started match */
	UFUNCTION(reliable, client)
	void ClientGameStarted();

	//Reset stats for testing
	void ResetGameStats();

	virtual void SetupInputComponent() override;



	// For tracking whether or not to send the end event
	bool bHasSentStartEvents;

};
