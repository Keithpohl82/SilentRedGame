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


	////// SettingsMain Menu //////

	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsMain;

	UPROPERTY(meta = (BindWidget))
	UButton* Settings_MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* Settings_MPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* Settings_SinglePlayer;

	UPROPERTY(meta = (BindWidget))
	UButton* Settings_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* Settings_QuitBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* Video_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Audio_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Mouse_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Keyboard_Button;


	////// SettingsVideo Menu //////
	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsVideo;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsV_MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsV_MPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsV_SinglePlayer;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsV_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsV_QuitBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* VideoV_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* AudioV_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* MouseV_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* KeyboardV_Button;


	////// SettingsAudio Menu //////
	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsAudio;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsA_MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsA_MPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsA_SinglePlayer;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsA_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsA_QuitBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* AudioA_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* MouseA_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* KeyboardA_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* VideoA_Button;


	////// SettingsMouse Menu //////
	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsMouse;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsM_MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsM_MPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsM_SinglePlayer;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsM_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsM_QuitBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* KeyboardM_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* VideoM_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* MouseM_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* AudioM_Button;


	////// SettingsKeyboard Menu //////
	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsKeyboard;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsK_MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* Settings_K_MPBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsK_SinglePlayer;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsK_ProfileBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsK_QuitBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* KeyboardK_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* VideoK_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* MouseK_Button;

	UPROPERTY(meta = (BindWidget))
	UButton* AudioK_Button;


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

	UFUNCTION()
	void OpenSettingsMenu();
	UFUNCTION()
	void OpenVideoSettingsMenu();
	UFUNCTION()
	void OpenAudioSettingsMenu();
	UFUNCTION()
	void OpenMouseSettingsMenu();
	UFUNCTION()
	void OpenKeyboardSettingsMenu();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostServerMenu();

	UFUNCTION()
	void QuitGame();

	TOptional<uint32> SelectedServerIndex;

	void UpdateChildren();

};