// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "BasePistol.generated.h"





UCLASS()
class SILENTRED_API ABasePistol : public ABaseWeapon
{
	GENERATED_BODY()
	


protected:

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = Projectiles)
	TSubclassOf<AActor> ProjectileClass;

public:


};
