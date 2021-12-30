// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class UHealthComponent;
class UMaterialInstanceConstant;
class ABasePlayerState;
// class ABaseWeapon;
class AMasterWeapon;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBaseCharacterEquipWeapon, ABaseCharacter*, AMasterWeapon* /* new */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBaseCharacteUnEquipWeapon, ABaseCharacter*, AMasterWeapon* /* old */);

UCLASS()
class SILENTRED_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

		/** spawn inventory, setup initial variables */
		virtual void PostInitializeComponents() override;

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

	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class AMasterWeapon* NewWeapon);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNextWeapon();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPreviousWeapon();


	/** updates current weapon */
	void SetCurrentWeapon(class AMasterWeapon* NewWeapon, class AMasterWeapon* LastWeapon = NULL);
	void EquipWeapon( class AMasterWeapon* Weapon);

	/** current weapon rep handler */
	UFUNCTION()
	void OnRep_CurrentWeapon(class AMasterWeapon* LastWeapon);

	/** cleanup inventory */
	virtual void Destroyed() override;

	/** get total number of inventory items */
	int32 GetInventoryCount() const;

	class AMasterWeapon* GetInventoryWeapon(int32 index) const;
	
	void AddWeapon(class AMasterWeapon* Weapon);
	void RemoveWeapon(class AMasterWeapon* Weapon);
	class AMasterWeapon* FindWeapon(TSubclassOf<class AMasterWeapon> WeaponClass);
	void NextWeapon();
	void PreviousWeapon();

	/** [server] spawns default inventory */
	void SpawnDefaultInventory();

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();

	UFUNCTION()
	void OnHealthChanged(UPlayerHealthComp* HealthComp, float Health, float Armor, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);




	UPROPERTY(EditDefaultsOnly, Category = Health)
	UPlayerHealthComp* PlayerHealthComp;

	/* Holds all weapons in players selected loadout*/
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<AMasterWeapon>> WeaponLoadout;

	TArray<AMasterWeapon*> Inventory;

	UPROPERTY(Replicated, BlueprintReadOnly)
	AMasterWeapon* CurrentWeapon;

	UPROPERTY(BlueprintReadOnly)
	bool bDied;

public:	

	UPROPERTY(EditDefaultsOnly, Replicated, Category = Animations)
	UAnimSequence* DeathAnim;

	FName GetWeaponAttachPoint() const;

	bool CanFire();
	bool CanReload();

	USkeletalMeshComponent* GetSpecificPawnMesh(bool WantFirstPerson) const;
	USkeletalMeshComponent* GetPawnMesh() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetPawnViewLocation()const override;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Teams)
	TArray<UMaterialInstanceConstant*> PlayerSkins;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleDefaultsOnly, Category = Weapons)
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName UnEquippedSocket;

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetHealth();

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetArmor();

	int32 GetTeamNumber();

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TeamNum;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Weapons)
	int32 WeaponIndex;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsFlagOwner;

	static FOnBaseCharacterEquipWeapon NotifyEquipWeapon;
	static FOnBaseCharacteUnEquipWeapon NotifyUnEquipWeapon;
	

};
