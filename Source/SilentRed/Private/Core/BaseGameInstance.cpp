// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/BaseGameInstance.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/MenuSystem/MainMenu.h"
#include "SilentRed/Public/MenuSystem/MenuWidget.h"
#include "Engine/Engine.h"

#include "Misc/DateTime.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "TimerManager.h"
#include "Engine/DemoNetDriver.h"
#include "Kismet/GameplayStatics.h"

#include "steam/isteammatchmaking.h"
#include "steam/isteamfriends.h"
#include "steam/isteamnetworking.h"
#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"

const static FName SESSION_NAME = TEXT("Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("Server Name");



UBaseGameInstance::UBaseGameInstance(const FObjectInitializer& ObjectInitalizer)
{
	
	bool bRet = SteamAPI_Init();

	if (bRet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Steam API was Initialilzed"));

	}
	
	ConstructorHelpers::FClassFinder<UUserWidget>MenuBPClass(TEXT("/Game/UI/MenuSystem/BP_MainMenu"));
	if(!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget>InGameMenuBPClass(TEXT("/Game/UI/MenuSystem/BP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;
	InGameMenuClass = InGameMenuBPClass.Class;

	RecordingName = FDateTime::Now().ToString();
	FriendlyRecordingName = "MyReplay";

}

void UBaseGameInstance::RemoveExistingLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	check(ExistingPlayer);
	if (ExistingPlayer->PlayerController != NULL)
	{
		// Kill the player
		
	}

	// Remove local split-screen players from the list
	RemoveLocalPlayer(ExistingPlayer);
}

void UBaseGameInstance::RemoveSplitScreenPlayers()
{
	// if we had been split screen, toss the extra players now
	// remove every player, back to front, except the first one
	while (LocalPlayers.Num() > 1)
	{
		ULocalPlayer* const PlayerToRemove = LocalPlayers.Last();
		RemoveExistingLocalPlayer(PlayerToRemove);
	}
}

void UBaseGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Subsystem Interface"));

			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBaseGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UBaseGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBaseGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UBaseGameInstance::OnJoinSessionComplete);
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Subsystem"));
	}
	UE_LOG(LogTemp, Warning, TEXT("Found Class %s"), *MenuClass->GetName());

}

void UBaseGameInstance::LoadMainMenu()
{
	if (!ensure(MenuClass != nullptr)) return;

	_Menu = CreateWidget<UMainMenu>(this, MenuClass);

	if (!ensure(_Menu != nullptr)) return;

	_Menu->bIsFocusable = true;
	
	_Menu->Setup();

	_Menu->SetMenuInterface(this);
}

void UBaseGameInstance::InGameLoadMainMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	UMenuWidget* _Menu = CreateWidget<UMenuWidget>(this, InGameMenuClass);

	if (!ensure(_Menu != nullptr)) return;

	_Menu->bIsFocusable = true;

	_Menu->Setup();

	_Menu->SetMenuInterface(this);
}

void UBaseGameInstance::HostServer(FString ServerName)
{

	DesiredServerName = ServerName;

	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);

		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UBaseGameInstance::JoinServer(uint32 Index)
{

	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	if (_Menu != nullptr)
	{
		_Menu->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
	
}


bool UBaseGameInstance::InvitePlayerToLobby(FString PlayerToInvite)
{
	bool InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee);
	
	return true;
}

void UBaseGameInstance::CreateGameLobby()
{
	
	SteamAPICall_t CreateLobby(k_ELobbyTypePublic);

	if (k_EResultOK)
	{
		UE_LOG(LogTemp, Warning, TEXT("Lobby was created"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED to create lobby"));
	}
}



void UBaseGameInstance::GetListOfLobbies()
{
	SteamMatchmaking()->AddRequestLobbyListResultCountFilter(10);

	SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
	m_CallResultLobbyMatchList.Set(hSteamAPICall, this, &UBaseGameInstance::OnSearchLobbyComplete);
}

void UBaseGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		FOnlineSessionSettings SessionSettings;

		SessionSearch->MaxSearchResults = 1000;
		
		
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		
		UE_LOG(LogTemp, Warning, TEXT("Starting To Find Sessions"));
		SessionSearch->bIsLanQuery = true;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}

}

void UBaseGameInstance::BackToMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel("/Game/Maps/MainMenuLevel", ETravelType::TRAVEL_Absolute);
}

void UBaseGameInstance::StartRecording(const FString& InName, const FString& FriendlyName)
{
	RecordingName = GetWorld()->GetMapName() + FriendlyRecordingName;

	StartRecordingReplay(RecordingName,	FriendlyRecordingName);
	UE_LOG(LogTemp, Warning, TEXT("Starting to record your demo"));
}

void UBaseGameInstance::StopRecording()
{
	StopRecordingReplay();
	UE_LOG(LogTemp, Warning, TEXT("Stopped recording your demo"));
}

void UBaseGameInstance::StartReplay()
{
	PlayReplay(RecordingName, nullptr);
}

FString UBaseGameInstance::GetIPAddress()
{
	return GetWorld()->GetAddressURL();
}



void UBaseGameInstance::OnSearchLobbyComplete( LobbyMatchList_t	*pLobbyMatchList, bool bIOFailure)
{
	if (bIOFailure)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to retrive list of lobbies!!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Lobby ID is %i"));
	
	
	/*for (var i = pLobbyMatchList; i = 0 ; pLobbyMatchList < pLobbyMatchList)
	{
		SteamMatchmaking()->GetLobbyByIndex();
		UE_LOG(LogTemp, Warning, TEXT("Lobby ID is %i"), pLobbyMatchList);
	}*/
}

void UBaseGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{

		
		FOnlineSessionSettings SessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == NULL_SUBSYSTEM)
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}

		SessionSettings.bIsLANMatch = bIsALanMatch;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.NumPublicConnections = MaxNumberOfPlayers;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence =true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);

		// FOR TESTING DEDICATED SERVER
		UE_LOG(LogTemp, Warning, TEXT("Create Session was called"));
	}
}

void UBaseGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{

	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
	}

	if (_Menu != nullptr)
	{
		_Menu->Teardown();
	}

	UE_LOG(LogTemp, Warning, TEXT("Create Session COMPLETE was called"));

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;


	World->ServerTravel("/Game/Maps/JordanBlock?listen");


}

void UBaseGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
}

void UBaseGameInstance::OnFindSessionsComplete(bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("Finished Finding Sessions"));

	
	if (Success && SessionSearch.IsValid() && _Menu != nullptr)
	{

		TArray<FServerData> ServerNames;

		//ServerNames.Add("Test Server 1");
		//ServerNames.Add("Test Server 2");
		//ServerNames.Add("Test Server 3");
		//ServerNames.Add("Test Server 4");

		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Sessions Name: %s"), *SearchResult.GetSessionIdStr());
			
			FServerData Data;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUser = SearchResult.Session.OwningUserName;
			Data.ServerPing = SearchResult.PingInMs;
			

			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.ServerName = ServerName;
			}
			else
			{
				Data.ServerName = "Could not get server name";
			}
			ServerNames.Add(Data);
		}
		_Menu->SetServerList(ServerNames);
	}
}

void UBaseGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{

	if (!SessionInterface.IsValid()) return;

	FString Address;
	
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connection string"));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}


