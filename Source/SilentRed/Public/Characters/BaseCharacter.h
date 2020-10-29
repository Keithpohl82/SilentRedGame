// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UWeaponInventoryComponent;
class UHealthComponent;
class UMaterialInstanceConstant;
class ABasePlayerState;
class ABaseWeapon;

UCLASS()
class SILENTRED_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, Category = Camera)
		UCameraComponent* CameraComp;

	

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	


	UPROPERTY(EditDefaultsOnly, Category = "Flag")
	FName FlagAttachSocketName;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/* Movement and input */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();
	void StartFire();
	void StopFire();
	void ReloadGun();


	/* Replication server side functions */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNextWeapon();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPreviousWeapon();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnWeapon();



	void WeaponToSpawn();
	void NextWeapon();
	void PreviousWeapon();

	UFUNCTION()
	void OnHealthChanged(UPlayerHealthComp* HealthComp, float Health, float Armor, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	UPROPERTY(Replicated, BlueprintReadOnly)
	ABaseWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	UPlayerHealthComp* PlayerHealthComp;

	/* Holds all weapons in players selected loadout*/
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ABaseWeapon>> WeaponLoadout;


	UPROPERTY(Replicated, BlueprintReadWrite)
	ABaseWeapon* GunToSpawn;

	//UPROPERTY(EditDefaultsOnly, Category = Weapons)
	//TSubclassOf<ABaseWeapon> StarterWeaponClass;



	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetPawnViewLocation()const override;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Teams)
	TArray<UMaterialInstanceConstant*> PlayerSkins;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleDefaultsOnly, Category = Weapons)
	FName WeaponAttachSocketName;

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetHealth();

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetArmor();

	int32 GetTeamNumber();

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TeamNum;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Weapons)
	int32 WeaponIndex;
	

};
