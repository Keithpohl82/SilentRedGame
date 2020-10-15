// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;
class ABaseWeapon;
class UHealthComponent;
class UMaterialInstanceConstant;


UCLASS()
class SILENTRED_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, Category = Camera)
		UCameraComponent* CameraComp;

	

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	//USkeletalMeshComponent* Mesh1P;

		UPROPERTY(BlueprintReadOnly)
		int32 TeamNum = 0;

		UPROPERTY(EditDefaultsOnly, Category = "Flag")
		FName FlagAttachSocketName;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();
	void StartFire();
	void StopFire();
	void ReloadGun();

	void SetPlayerSkin();


	UFUNCTION()
	void OnHealthChanged(UHealthComponent* HealthComp, float Health, float Armor, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	UPROPERTY(Replicated, BlueprintReadOnly)
	ABaseWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	UHealthComponent* PlayerHealthComp;

	UPROPERTY(EditDefaultsOnly, Category = Weapons)
	TSubclassOf<ABaseWeapon> StarterWeaponClass;

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = Teams)
	TArray<UMaterialInstanceConstant*> PlayerSkins;

	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetPawnViewLocation()const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(VisibleDefaultsOnly, Category = Weapons)
	FName WeaponAttachSocketName;

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetHealth();

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetArmor();

	UFUNCTION(BlueprintCallable)
	int32 GetTeamColor();

	

};
