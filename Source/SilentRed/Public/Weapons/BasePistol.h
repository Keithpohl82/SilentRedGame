// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "BasePistol.generated.h"





UCLASS()
class SILENTRED_API ABasePistol : public ABaseWeapon
{
	GENERATED_BODY()
	
	ABasePistol();

protected:

	virtual void Fire() override;



public:




};
