// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/MenuSystem/ServerRow.h"
#include "Components/Button.h"
#include "SilentRed/Public/MenuSystem/MainMenu.h"

void UServerRow::Setup(UMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
	RowBtn->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
	Parent->SelectServerIndex(Index);
}