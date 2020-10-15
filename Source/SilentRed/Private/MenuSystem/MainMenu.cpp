// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/MenuSystem/MainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "SilentRed/Public/MenuSystem/MainMenuInterface.h"

#include "SilentRed/Public/MenuSystem/ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget>ServerRowBPClass(TEXT("/Game/UI/MenuSystem/BP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;
	ServerRowClass = ServerRowBPClass.Class;
}



void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerListBox->ClearChildren();

	uint32 i = 0;

	for (const FServerData& ServerData : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(this, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->ServerName->SetText(FText::FromString(ServerData.ServerName));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUser));
		FString ServerPing = FString::Printf(TEXT("%d"), ServerData.ServerPing);
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Row->ConnectionFraction->SetText(FText::FromString(FractionText));
		Row->ServerPing->SetText(FText::FromString(ServerPing));
		Row->Setup(this, i);
		i++;
		ServerListBox->AddChild(Row);
	}
}

void UMainMenu::SelectServerIndex(uint32 Index)
{
	SelectedServerIndex = Index;
	UpdateChildren();
}

bool UMainMenu::Initialize()
{
	bool Sucess = Super::Initialize();
	if (!Sucess) return false;

	
	

	//Main Menu//
	MM_SPBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenSinglePlayerMenu);
	MM_MPBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenMultiPlayerMenu);
	//MM_SettingsBtn
	//MM_ProfileBtn
	MM_QuitBtn->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);


	//Multiplayer Menu
	MP_HostServerBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenHostServerMenu);
	MP_JoinServerBtn->OnClicked.AddDynamic(this, &UMainMenu::JoinIPAddress);
	MP_SPBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenSinglePlayerMenu);
	MP_MainMenuBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	MP_JoinIPBtn->OnClicked.AddDynamic(this, &UMainMenu::JoinIPAddress);
	MP_RefreshServersBtn->OnClicked.AddDynamic(this, &UMainMenu::RefreshServers);
	//MP_SettingsBtn
	//MP_ProfileBtn
	MP_QuitGameBtn->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	//Single Player Menu
	SP_MainMenuBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	//SP_SettingsMenu
	SP_MPBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenMultiPlayerMenu);
	//SP_ProfileBtn
	SP_QuitBtn->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);


	//Host Server Menu
	if (!ensure(HS_StartServerBtn != nullptr)) return false;
	HS_StartServerBtn->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	HS_SPBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenSinglePlayerMenu);
	HS_MainMenuBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	//HS_SettingsBtn
	//HS_ProfileBtn
	HS_JoinServerBtn->OnClicked.AddDynamic(this, &UMainMenu::OpenMultiPlayerMenu);
	HS_QuitBtn->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);


	return true;
	
	
}

void UMainMenu::HostServer()
{
	UE_LOG(LogTemp, Warning, TEXT("I'm going to host a server!!"))


	if (MenuInterface != nullptr)
	{
		FString ServerName = ServerNameBox->Text.ToString();
		MenuInterface->HostServer(ServerName);
	}
}



void UMainMenu::JoinIPAddress()
{
	if (SelectedServerIndex.IsSet() && MenuInterface != nullptr)
	{
		MenuInterface->JoinServer(SelectedServerIndex.GetValue());
		UE_LOG(LogTemp, Warning, TEXT("Selected Index %d"), );
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index NOT SET!!"));
	}
}

void UMainMenu::OpenSinglePlayerMenu()
{
	MenuSwitcher->SetActiveWidget(SinglePlayer);
}

void UMainMenu::OpenMultiPlayerMenu()
{
	MenuSwitcher->SetActiveWidget(MultiPlayer);
	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
}

void UMainMenu::RefreshServers()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
}

void UMainMenu::OpenMainMenu()
{
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenHostServerMenu()
{
	MenuSwitcher->SetActiveWidget(HostServerMenu);
}


void UMainMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if(!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ConsoleCommand("quit");
}

void UMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerListBox->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow>(ServerListBox->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedServerIndex.IsSet() && SelectedServerIndex.GetValue() == i);
		}
	}
}

