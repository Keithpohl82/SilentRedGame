// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCurveFloat;

USTRUCT()
struct FWeaponTrace 
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
	FVector_NetQuantize TraceTo;

};

UCLASS(Config=WeaponConfig)
class SILENTRED_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* MeshComp;

	
	void Fire();
	

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadWeapon();

	void PlayWeaponEffects(FVector TraceEnd);
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	//Needs to be replicated and called on server
	//to avoid cheating by removing recoil
	void Recoil();
	void ResetRecoil();

	// Base Damage amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponConfig)
	float BaseDamageAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponConfig)
	TSubclassOf<UDamageType> DamageType;
	//Name of the weapon
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfig)
	FName WeaponName;
	//Damage multiplier for head shots. Must be > 1
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfig)
	float HeadShotMultiplyer;
	//Amount of ammo in a Magazine
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 ClipAmmo;
	//Starting amount of ammo in a clip
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = WeaponConfig)
	int32 DefaultClipAmmo;
	//Max amount of ammo a weapon can hold
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MaxAmmo;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = WeaponConfig)
	int32 DefaultMaxAmmo;
	//Max distance a projectile will go
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfig)
	float WeaponRange;
	//Name of Muzzle socket for particle EFX, Line Trace, and Projectile
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfig)
	FName MuzzleSocketName;
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfig)
	FName TracerTargetName;

	//Recoil system needs more understanding and testing
	//to better understand the system
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	UCurveFloat* VerticalRecoil;
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	UCurveFloat* HorizontalRecoil;
	//Below values need to be replicated from server
	//to prevent cheating
	
	//Amount of time before Recoil curve restarts
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float RecoilRecoveryTime;

	//UPROPERTY(EditDefaultsOnly, Category = Recoil)
	//float RecoilRecoveryDelay;

	//How much or little the crosshair moves each shot
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float RecoilMultiplyer;
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float RecoilTimePerShot;
	//Testing 
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float DefaultRecoilTimePerShot;


	UPROPERTY(EditDefaultsOnly, Category = EFX)
	UParticleSystem* MuzzleEffect;
	UPROPERTY(EditDefaultsOnly, Category = EFX)
	UParticleSystem* DefaultImpactEffect;
	UPROPERTY(EditDefaultsOnly, Category = EFX)
	UParticleSystem* FleshImpactEffect;
	UPROPERTY(EditDefaultsOnly, Category = EFX)
	UParticleSystem* TracerEffect;


	FTimerHandle TimerHandle_TimeBetweenShots;
	FTimerHandle TimerHandle_RecoilReset;

	float LastFireTime;

	//Determines how fast or slow the weapon will fire multiple shots
	UPROPERTY(EditDefaultsOnly, Category = WeaponConfig)
	float RateOfFire;

	float TimeBetweenShots;



	UPROPERTY(ReplicatedUsing=OnRep_WeaponTrace)
	FWeaponTrace WeaponTrace;
	UFUNCTION()
	void OnRep_WeaponTrace();



public:	

	void StartFire();
	void StopFire();
	void ReloadWeapon();

	void RecoilReset();




};
