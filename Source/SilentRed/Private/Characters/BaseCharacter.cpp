// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/MasterGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "SilentRed/Public/Weapons/BaseWeapon.h"
#include"SilentRed/SilentRed.h"
#include "SilentRed/Public/Components/PlayerHealthComp.h"
#include "SilentRed/Public/Components/WeaponInventoryComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetMesh());
	CameraComp->bUsePawnControlRotation = true;

	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	WeaponAttachSocketName = "WeaponSocket";

	GetCapsuleComponent()-> SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	PlayerHealthComp = CreateDefaultSubobject<UPlayerHealthComp>(TEXT("PlayerHealth"));

	WeaponIndex = 0;

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, CurrentWeapon);
	DOREPLIFETIME(ABaseCharacter, PlayerSkins);
	DOREPLIFETIME(ABaseCharacter, TeamNum);
	DOREPLIFETIME(ABaseCharacter, WeaponLoadout);
	DOREPLIFETIME(ABaseCharacter, GunToSpawn);
	DOREPLIFETIME(ABaseCharacter, WeaponIndex);

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	ABasePlayerState* PS = Cast<ABasePlayerState>(GetPlayerState());
	if (PS != nullptr)
	{
		TeamNum = PS->GetTeamNumber();
	}
	

	PlayerHealthComp->OnHealthChanged.AddDynamic(this, &ABaseCharacter::OnHealthChanged);
	WeaponToSpawn();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UWorld* World = GetWorld();

	
}

FVector ABaseCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
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



void ABaseCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ABaseCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ABaseCharacter::ReloadGun()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->ReloadWeapon();
	}
}



void ABaseCharacter::ServerNextWeapon_Implementation()
{
	NextWeapon();
}

bool ABaseCharacter::ServerNextWeapon_Validate()
{
	return true;
}

void ABaseCharacter::ServerPreviousWeapon_Implementation()
{
	PreviousWeapon();
}

bool ABaseCharacter::ServerPreviousWeapon_Validate()
{
	return true;
}

void ABaseCharacter::ServerSpawnWeapon_Implementation()
{
	WeaponToSpawn();
}

bool ABaseCharacter::ServerSpawnWeapon_Validate()
{
	return true;
}

void ABaseCharacter::WeaponToSpawn()
{
	if (!HasAuthority())
	{
		ServerSpawnWeapon();
	}

	FName WeaponSocket = "WeaponSocket";

	GunToSpawn = GetWorld()->SpawnActor<ABaseWeapon>(WeaponLoadout[WeaponIndex]);
	GunToSpawn->SetOwner(this);
	GunToSpawn->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
	CurrentWeapon = GunToSpawn;
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, FString::Printf(TEXT("WeaponToSpawn")));
}

void ABaseCharacter::NextWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerNextWeapon();
	}
	if (WeaponLoadout.Num() > 0)
	{
		WeaponIndex++;
		if (WeaponIndex == WeaponLoadout.Num())
		{
			// set visibility instead of destroy?
			WeaponIndex = 0;

			CurrentWeapon->Destroy();
			WeaponToSpawn();
		}
		// set visibility instead of destroy?
		CurrentWeapon->Destroy();
		WeaponToSpawn();
	}
}

void ABaseCharacter::PreviousWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerPreviousWeapon();
	}
	if (WeaponLoadout.Num() > 1)
	{
		if (WeaponIndex > 0)
		{
			// set visibility instead of destroy?
			WeaponIndex--;

			CurrentWeapon->Destroy();
			WeaponToSpawn();
		}
		else
		{
			// set visibility instead of destroy?
			WeaponIndex = WeaponLoadout.Num() - 1;

			CurrentWeapon->Destroy();
			WeaponToSpawn();
		}
	}
}

void ABaseCharacter::OnHealthChanged(UPlayerHealthComp* HealthComp, float Health, float Armor, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetLifeSpan(5.0f);

		AMasterPlayerController* PC = Cast<AMasterPlayerController>(GetOwner());
		PC->Deaths++;

		DetachFromControllerPendingDestroy();

		AMasterGameMode* GM = Cast<AMasterGameMode>(GetWorld()->GetAuthGameMode());
		GM->RestartDeadPlayer();

	}
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

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ABaseCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ABaseCharacter::StopFire);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ABaseCharacter::ReloadGun);

	PlayerInputComponent->BindAction(TEXT("NextWeapon"), IE_Pressed, this, &ABaseCharacter::NextWeapon);
	PlayerInputComponent->BindAction(TEXT("PreviousWeapon"), IE_Pressed, this, &ABaseCharacter::PreviousWeapon);

}

float ABaseCharacter::GetHealth()
{
	return PlayerHealthComp->Health;
}

float ABaseCharacter::GetArmor()
{
	return PlayerHealthComp->Armor;
}

int32 ABaseCharacter::GetTeamNumber()
{
	ABasePlayerState* PS = Cast<ABasePlayerState>(GetPlayerState());
	return PS->TeamNumber;
}



