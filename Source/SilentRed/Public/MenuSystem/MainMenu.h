// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SilentRed/Public/MenuSystem/MenuWidget.h"
#include "SilentRed/Public/MenuSystem/MainMenuInterface.h"



#include "MainMenu.generated.h"


class UButton;
class UWidget;
class UWidgetSwitcher;
class UEditableTextBox;
class UPanelWidget;


USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString ServerName;
	FString HostUser;
	uint32 CurrentPlayers;
	uint32 MaxPlayers;
	uint32 ServerPing;
	//FString MapName;

};



/**
 * 
 */
UCLASS()
class SILENTRED_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:


	UMainMenu(const FObjectInitializer& ObjectInitializer);

	void SetServerList(TArray<FServerData> ServerNames);

	void SelectServerIndex(uint32 Index);

protected:

	virtual bool Initialize() override;


private:


	
	//////Main Menu//////

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* MM_SPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MM_MPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MM_SettingsBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MM_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MM_QuitBtn;



	//////Multiplayer Menu//////
	UPROPERTY(meta = (BindWidget))
	UWidget* MultiPlayer;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ServerListBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* IPAddressField;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_JoinServerBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_SPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_MainMenuBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_SettingsBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_HostServerBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_QuitGameBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_JoinIPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* MP_RefreshServersBtn;


	////// Single Player Menu //////

	UPROPERTY(meta = (BindWidget))
	UWidget* SinglePlayer;

	UPROPERTY(meta = (BindWidget))
	UButton* SP_MainMenuBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SP_SettingsMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* SP_MPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SP_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SP_QuitBtn;



	////// Host Server Menu //////

	UPROPERTY(meta = (BindWidget))
	UWidget* HostServerMenu;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ServerNameBox;
	
	UPROPERTY(meta = (BindWidget))
	UButton* HS_StartServerBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HS_SPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HS_MainMenuBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HS_SettingsBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HS_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HS_JoinServerBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HS_QuitBtn;



	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;


	TSubclassOf<UUserWidget> ServerRowClass;
	
	
	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinIPAddress();

	UFUNCTION()
	void OpenSinglePlayerMenu();

	UFUNCTION()
	void OpenMultiPlayerMenu();

	UFUNCTION()
	void RefreshServers();

	//UFUNCTION()
	//void OpenProfileMenu();

	//UFUNCTION()
	//void OpenSettingsMenu();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostServerMenu();

	UFUNCTION()
	void QuitGame();

	TOptional<uint32> SelectedServerIndex;

	void UpdateChildren();

};