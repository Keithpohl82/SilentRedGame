// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

//forward declarations
class UCameraComponent;

UCLASS()
class SILENTRED_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Animations)
	UAnimSequence* DiveAnim;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category = "Player")
	USkeletalMeshComponent* Mesh1P;

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Movement and inputs
	void MoveForward(float value);
	void MoveRight(float value);
	void BeginCrouch();
	void EndCrouch();
	void Dive();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
