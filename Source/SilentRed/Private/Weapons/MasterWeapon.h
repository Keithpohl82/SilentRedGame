// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "MasterWeapon.generated.h"

class USkeletalMeshComponent;
class UCurveFloat;
class UDamageType;
class UParticleSystem;

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

		//Time between shots for Rate of fire
		UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float TimeBetweenShots;

	//Timer for reloading weapon 
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float ReloadDuration;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float RecoilTimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float BulletEndPoint;

	// Bullets fired per minuet
	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		float RateOfFire;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
		int32 ShotgunPellets;


	//SetsDefaults
	FWeaponData()
	{
		TimeBetweenShots = 0;
		ReloadDuration = 0;
		RecoilTimeBetweenShots = 0.0f;
		BulletEndPoint = 0;
		RateOfFire = 0;
		ShotgunPellets = 0;
	}
};


USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	//UPROPERTY()
	//TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class AMasterWeapon : public AActor
{
	GENERATED_BODY()

		/* Current Weapon State */
		EWeaponState::Type CurrentState;

	/** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ABaseCharacter* MyPawn;

	UFUNCTION()
	void OnRep_MyPawn();

	/* [local + server] interrupt weapon reload */
	 void StopReload();

	/* Updates the weapon's state */
	void SetWeaponState(EWeaponState::Type NewState);

	/* check if weapon can fire */
	bool CanFire() const;

	/* check if weapon can be reloaded */
	bool CanReload() const;

	/* Determines if 3rd person or 1st person mesh to spawn */
	USkeletalMeshComponent* GetWeaponMesh() const;
	
public:
	// Sets default values for this actor's properties
	AMasterWeapon();

	/* [all] start weapon reload */
	 void StartReload(bool bFromReplication = false);

	/* Current total ammo */
	UPROPERTY(transient, Replicated)
	int32 CurrentAmmo;

	/* Current ammo - inside clip*/
	UPROPERTY(Transient, Replicated)
	int32  CurrentAmmoInClip;

	//Amount of ammo in a clip
	UPROPERTY(Replicated, EditDefaultsOnly, Category = WeaponStats)
	int32 AmmoPerClip;

	//Starting number of clips
	UPROPERTY(Replicated, EditDefaultsOnly, Category = WeaponStats)
	int32 InitialClips;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Ammo")
	int Ammo;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = WeaponStats)
	int32 BackupAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStats)
	FName WeaponName;

	//////////////////////////////////////////////////////////////////////////
	//Inventory

	/** weapon is being equipped by owner pawn */
	 void OnEquip(const AMasterWeapon* LastWeapon);

	/** weapon is now equipped by owner pawn */
	void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	 void OnUnEquip();

	/** [server] weapon was added to pawn's inventory */
	 void OnEnterInventory(class ABaseCharacter* NewOwner);

	/** [server] weapon was removed from pawn's inventory */
	 void OnLeaveInventory();

	/** check if it's currently equipped */
	bool IsEquipped() const;

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

	/* Getter for the current weapon state */
	EWeaponState::Type GetCurrentState() const;

	/* Sets who the owner of the weapon is */
	void SetOwningPawn(class ABaseCharacter* ABaseCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* MeshComp;

	/* Attaches weapon mesh to the player pawn*/
	void AttachMeshToPawn();

	/* Detaches weapon mesh to the player pawn*/
	void DetachMeshFromPawn();

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	void Fire();

	void Recoil();

	void ResetRecoil();

	/* Determines what state the weapon is in, See Weapon state enum*/
	void DeterMineWeaponState();

	UPROPERTY(replicated)
	int AmountToReload;

	UPROPERTY()
	float RecoilTimePerShot;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
		FName EjectSocketName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
		FName MuzzleSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Effects")
		FName TracerTargetName;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
		UParticleSystem* MuzzleEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
		UParticleSystem* EjectEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
		UParticleSystem* DefaultImpactEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
		UParticleSystem* FleshImpactEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
		UParticleSystem* TracerEffect;
	UPROPERTY(EditAnywhere, Category = "Weapon Sounds")
		USoundBase* MuzzleSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Sounds")
		USoundBase* ImpactSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Sounds")
		USoundBase* ReloadSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Sounds")
		USoundBase* OutOfAmmoSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float HeadShotBonus;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		UCurveFloat* VerticalRecoil;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		UCurveFloat* HorizontalRecoil;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		float RecoilRecoveryTime;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		float RecoilRecoveryDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		float RecoilMultiplyer;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
		bool isAuto;




	class ABaseCharacter* PlayerCharacter;

	FTimerHandle TimerHandle_TimeBetweenShots;

	FTimerHandle TimerHandle_OnEquipFinished;

	FTimerHandle TimerHandle_StopReload;

	FTimerHandle TimerHandle_ReloadWeapon;

	float LastFireTime;

	float TimeBetweenShots;




	/* is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	/* is weapon fire active? */
	uint32 bWantsToFire : 1;

	/* is weapon being equipped? */
	uint32 bPendingEquip : 1;

	/*is weapon reloading? ;*/
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Reload)
	uint32 bPendingReload : 1;


	/* When the Weapon was switched to*/
	float EquipStartedTime;

	/* How much time the weapon needs to be equipped*/
	float EquipDuration;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	bool bIsShotGun;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartFire();

	void StopFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadGun();

	void ServerReloadGun_Implementation();

	void Reload();

	UFUNCTION(BlueprintPure)
	int GetAmmo();

	UFUNCTION(BlueprintPure)
	int GetTotalAmmo();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	void ServerFire_Implementation();

	UFUNCTION()
	void OnRep_HitScanTrace();

	UFUNCTION()
	void OnRep_Reload();

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

};
