// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MasterPlayerController.generated.h"

//NOT REPLICATED players only know of their own.

/**
 * 
 */
UCLASS()
class SILENTRED_API AMasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	

	UFUNCTION(BlueprintCallable, Category = Replays)
	void RestartRecording();

public:

	UPROPERTY(Replicated)
	int32 TeamNum = 0;

	

};
