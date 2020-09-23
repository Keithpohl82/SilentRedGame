// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(CameraComp);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);


	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ABaseCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

void ABaseCharacter::BeginCrouch()
{
Crouch();
}

void ABaseCharacter::EndCrouch()
{
	UnCrouch();
}

void ABaseCharacter::Dive()
{
	
	bool bLoop = false;
	GetMesh()->PlayAnimation(DiveAnim, bLoop);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ABaseCharacter::BeginCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ABaseCharacter::EndCrouch);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Dive"), IE_Pressed, this, &ABaseCharacter::Dive);
}

