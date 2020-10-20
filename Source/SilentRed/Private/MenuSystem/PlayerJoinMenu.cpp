// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/MenuSystem/PlayerJoinMenu.h"
#include "Components/Button.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"

bool UPlayerJoinMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(BlueTeamBtn != nullptr)) return false;
	BlueTeamBtn->OnClicked.AddDynamic(this, &UPlayerJoinMenu::JoinBlueTeam);

	if (!ensure(RedTeamBtn != nullptr)) return false;
	RedTeamBtn->OnClicked.AddDynamic(this, &UPlayerJoinMenu::JoinRedTeam);

	return true;
}

void UPlayerJoinMenu::JoinRedTeam()
{

	//PS->SetTeamNum(1);
}

void UPlayerJoinMenu::JoinBlueTeam()
{
		//PS->SetTeamNum(2);
}
