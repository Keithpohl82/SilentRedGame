// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SilentRed/SilentRed.h"
#include "Engine/GameInstance.h"
#include "SilentRed/Public/MenuSystem/MainMenuInterface.h"
#include "NetworkReplayStreaming.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/NetworkDelegates.h"
#include "Steam/steam_api.h"

#include "BaseGameInstance.generated.h"


class UMainMenu;
class UUserWidget;
class ISteamMatchmaking;

//Demo and Replay system gos in here.



UCLASS(Config=Game)
class UBaseGameInstance : public UGameInstance, public IMainMenuInterface
{
public:
	GENERATED_BODY()
	
	UBaseGameInstance(const FObjectInitializer& ObjectInitalizer);

public:

	void RemoveExistingLocalPlayer(ULocalPlayer* ExistingPlayer);

	void RemoveSplitScreenPlayers();

	virtual void Init();

	// For logging in a player to the EOS Subsystem
	void Login();

	// Callback for when player login is completed
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	bool bIsLoggedIn;

	// Loads the main menu when finished initalizing
	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	// Loads the In game/ Match menu
	UFUNCTION(BlueprintCallable)
	void InGameLoadMainMenu();

	// For hosing a server on a PC
	UFUNCTION(Exec)
	void HostServer(FString ServerName) override;

	// Join the selected server
	UFUNCTION(Exec)
	void JoinServer(uint32 Index) override;

	// Invites a selected player from your friends list to lobby/party
	UFUNCTION(BlueprintCallable)
	bool InvitePlayerToLobby(FString PlayerToInvite);

	// Creates a lobby for other players to join
	UFUNCTION(BlueprintCallable)
	void CreateGameLobby();

	FString LobbyName;

	// Uses OnSearchLobbyComplete Call Return.
	UFUNCTION(BlueprintCallable)
	void GetListOfLobbies();
	
	CCallResult< UBaseGameInstance, LobbyMatchList_t > m_CallResultLobbyMatchList;

	//UFUNCTION()
	//void JoinServerIndex(uint32 Index) override;

	UPROPERTY(Config)
	int32 MaxNumberOfPlayers;

	// Refreshed the list of server that are avalible to join
	void RefreshServerList() override;

	// Called to take a player back to the main menu from the game or other menu
	virtual void BackToMainMenu() override;

	TArray<FString> MapsList;


	UPROPERTY(BlueprintReadWrite, Category = Sessions)
	bool bIsALanMatch;

	//////////////    Replay system   //////////////
	UPROPERTY(EditDefaultsOnly, Category = Replays)
	FString RecordingName;
	UPROPERTY(EditAnywhere, Category = Replays)
	FString FriendlyRecordingName;
	UFUNCTION(BlueprintCallable, Category = Replays)
	void StartRecording(const FString& InName, const FString& FriendlyName);
	UFUNCTION(BlueprintCallable, Category = Replays)
	void StopRecording();
	UFUNCTION(BlueprintCallable, Category = Replays)
	void StartReplay();
	//////////////    End Replay system   //////////////

	protected:

	// Gets the IP address of the server. For connecting from the console
	FString GetIPAddress();

	IOnlineSubsystem* Subsystem;


private:

	class UMainMenu* _Menu;
	
	// Call back for when finished searching for lobbies to join.
	void OnSearchLobbyComplete(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure);
	

	TSubclassOf<UUserWidget> MenuClass;
	TSubclassOf<UUserWidget> InGameMenuClass;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	UPROPERTY(Config)
	FString DesiredServerName;

	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

};
