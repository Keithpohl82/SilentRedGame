// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/SilentRedGameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubSystem.h"
#include "OnlineSessionSettings.h"
#include "Core/MasterGameMode.h"


ASilentRedGameSession::ASilentRedGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void ASilentRedGameSession::RegisterServer()
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			AMasterGameMode* MyGM = Cast<AMasterGameMode>(GetWorld()->GetAuthGameMode());
			if (MyGM)
			{
				
				SessionSettings->bUsesPresence = false;
				SessionSettings->bIsLANMatch = false;
				SessionSettings->bIsDedicated = true;
				SessionSettings->bShouldAdvertise = true;
				SessionSettings->NumPublicConnections = 25;
				Sessions->CreateSession(0, GameSessionName, *SessionSettings);
			}
		}
	}
}
