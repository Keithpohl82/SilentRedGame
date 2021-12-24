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

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
	void InGameLoadMainMenu();

	UFUNCTION(Exec)
	void HostServer(FString ServerName) override;

	UFUNCTION(Exec)
	void JoinServer(uint32 Index) override;

	UFUNCTION(BlueprintCallable)
	bool InvitePlayerToLobby(FString PlayerToInvite);

	UFUNCTION(BlueprintCallable)
	void CreateGameLobby();

	FString LobbyName;

	// Uses OnSearchLobbyComplete Call Return.
	UFUNCTION(BlueprintCallable)
	void GetListOfLobbies();
	

	//UFUNCTION()
	//void JoinServerIndex(uint32 Index) override;

	UPROPERTY(Config)
	int32 MaxNumberOfPlayers;

	void RefreshServerList() override;

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


	FString GetIPAddress();


private:

	class UMainMenu* _Menu;
	
	void OnSearchLobbyComplete(LobbyMatchList_t *pLobbyMatchList, bool bIOFailure);
	CCallResult< UBaseGameInstance, LobbyMatchList_t > m_CallResultLobbyMatchList;

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
