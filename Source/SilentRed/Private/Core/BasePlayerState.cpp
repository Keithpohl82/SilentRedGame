// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/MasterGameState.h"


ABasePlayerState::ABasePlayerState(const FObjectInitializer& ObjectInitializer)
{
	

	PlayerPing = GetPing();
	GamePlayerID = GetUniqueID();
	PlayersName = GetPlayerName();
	NumShotsFired = 0;


}

void ABasePlayerState::SetTeamNum(int32 NewTeamNumber)
{
	if (TeamNumber == 0)
	{
		TeamNumber = NewTeamNumber;
	}
}

void ABasePlayerState::OnRep_SetTeam(int32 NewNumber)
{
	SetTeamNum(TeamNumber);
}

void ABasePlayerState::SetTeamSkin()
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner());
	
}

int32 ABasePlayerState::GetTeamNumber() const
{
	return TeamNumber;
}

void ABasePlayerState::ShotFired()
{
	NumShotsFired += 1;
}




void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerState, TeamNumber);

	DOREPLIFETIME(ABasePlayerState, PlayerScore);

	DOREPLIFETIME(ABasePlayerState, NumKills);

	DOREPLIFETIME(ABasePlayerState, NumDeaths);

	DOREPLIFETIME(ABasePlayerState, PlayerKDA);
}

