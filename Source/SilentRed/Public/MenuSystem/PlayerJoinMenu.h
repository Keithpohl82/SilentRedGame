// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerJoinMenu.generated.h"


class UButton;
class ABasePlayerState;

/**
 * 
 */
UCLASS()
class SILENTRED_API UPlayerJoinMenu : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	virtual bool Initialize() override;

	

public:

	UPROPERTY(meta = (BindWidget))
	UButton* BlueTeamBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* RedTeamBtn;

	void JoinRedTeam();

	void JoinBlueTeam();

};
