// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Online.h"
#include "GameFramework/GameSession.h"
#include "SilentRedGameSession.generated.h"

/**
 * 
 */
UCLASS()
class SILENTRED_API ASilentRedGameSession : public AGameSession
{
	GENERATED_UCLASS_BODY()
	
	virtual void RegisterServer() override;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

};
