// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Core/BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "SilentRed/Public/Core/MasterGameMode.h"



ABasePlayerState::ABasePlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	TeamNumber = 0;
	PlayerPing = GetPing();
	GamePlayerID = GetUniqueID();
	PlayersName = GetPlayerName();
	NumShotsFired = 0;
	SetIsOnlyASpectator(true);
}

void ABasePlayerState::SetTeamNum(int32 NewTeamNumber)
{
	if (TeamNumber == 0)
	{
		TeamNumber = NewTeamNumber;
		
		AMasterGameMode* GM = Cast<AMasterGameMode>(GetWorld()->GetAuthGameMode());
		
		GM->RespawnPlayer();
	}
		SetIsOnlyASpectator(false);
		UE_LOG(LogTemp, Warning, TEXT("Your team number is %i"), TeamNumber);
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
	UE_LOG(LogTemp, Warning, TEXT("ShotFired from player state."))
}


void ABasePlayerState::InformAboutKill_Implementation(class ABasePlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ABasePlayerState* KilledPlayerState)
{
	//id can be null for bots
	if (KillerPlayerState->GetUniqueId().IsValid())
	{
		//search for the actual killer before calling OnKill()	
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AMasterPlayerController* TestPC = Cast<AMasterPlayerController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				// a local player might not have an ID if it was created with CreateDebugPlayer.
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
				FUniqueNetIdRepl LocalID = LocalPlayer->GetCachedUniqueNetId();
				if (LocalID.IsValid() && *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->GetUniqueId())
				{
					TestPC->OnKill();
				}
			}
		}
	}
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

