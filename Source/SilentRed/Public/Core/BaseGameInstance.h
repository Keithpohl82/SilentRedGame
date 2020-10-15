// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SilentRed/Public/MenuSystem/MainMenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BaseGameInstance.generated.h"


class UMainMenu;
class UUserWidget;

//Demo and Replay system gos in here.

/**
 * 
 */
UCLASS()
class SILENTRED_API UBaseGameInstance : public UGameInstance, public IMainMenuInterface
{
	GENERATED_BODY()
	
	

public:

	UBaseGameInstance(const FObjectInitializer & ObjectInitalizer);

	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
	void InGameLoadMainMenu();

	UFUNCTION(Exec)
	void HostServer(FString ServerName) override;

	UFUNCTION(Exec)
	void JoinServer(uint32 Index) override;

	//UFUNCTION()
	//void JoinServerIndex(uint32 Index) override;

	void RefreshServerList() override;

	virtual void BackToMainMenu() override;

	



	//////////////    Replay system   //////////////
	UPROPERTY(EditDefaultsOnly, Category = Replays)
	FString RecordingName;
	UPROPERTY(EditAnywhere, Category = Replays)
	FString FriendlyRecordingName;
	UFUNCTION(BlueprintCallable, Category = Replays)
	void StartRecording();
	UFUNCTION(BlueprintCallable, Category = Replays)
	void StopRecording();
	UFUNCTION(BlueprintCallable, Category = Replays)
	void StartReplay();
	//////////////    End Replay system   //////////////


private:

	class UMainMenu* _Menu;

	TSubclassOf<UUserWidget> MenuClass;
	TSubclassOf<UUserWidget> InGameMenuClass;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;


	FString DesiredServerName;
	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	

	


};
