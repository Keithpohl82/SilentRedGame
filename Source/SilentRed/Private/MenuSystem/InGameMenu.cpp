// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/MenuSystem/InGameMenu.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(ExitMenuBtn != nullptr)) return false;
	ExitMenuBtn->OnClicked.AddDynamic(this, &UInGameMenu::ExitMenuPressed);

	if (!ensure(InGameQuitBtn != nullptr)) return false;
	InGameQuitBtn->OnClicked.AddDynamic(this, &UInGameMenu::QuitGamePressed);

	return true;
}

void UInGameMenu::ExitMenuPressed()
{
	Teardown();
}

void UInGameMenu::QuitGamePressed()
{
	if (MenuInterface != nullptr)
	{
	//Get team number of player
	//Reduce the number of players on player number team
	Teardown();
		MenuInterface->BackToMainMenu();
	}
}
