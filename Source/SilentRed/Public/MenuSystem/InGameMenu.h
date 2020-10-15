// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SilentRed/Public/MenuSystem/MenuWidget.h"
#include "InGameMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class SILENTRED_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()
	
protected:

	virtual bool Initialize() override;

private:

		UPROPERTY(meta = (BindWidget))
		UButton* ExitMenuBtn;

		UPROPERTY(meta = (BindWidget))
		UButton* InGameQuitBtn;


		UFUNCTION()
		void ExitMenuPressed();

		UFUNCTION()
		void QuitGamePressed();

};
