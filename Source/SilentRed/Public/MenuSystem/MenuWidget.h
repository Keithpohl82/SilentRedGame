// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SilentRed/Public/MenuSystem/MainMenuInterface.h"


#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SILENTRED_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void SetMenuInterface(IMainMenuInterface* MenuInterface);

	void Setup();

	void Teardown();

private:

	


protected:

	IMainMenuInterface* MenuInterface;


};
