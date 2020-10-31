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
	
	SpawnInventory();
	
	//WeaponToSpawn();
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

void ABaseCharacter::ServerSpawnInventory_Implementation()
{
	SpawnInventory();
}

bool ABaseCharacter::ServerSpawnInventory_Validate()
{
	return true;
}

void ABaseCharacter::ServerSpawnPrimaryWeapon_Implementation()
{
	SpawnPrimaryWeapon();
}

bool ABaseCharacter::ServerSpawnPrimaryWeapon_Validate()
{
	return true;
}

void ABaseCharacter::ServerSpawnSecondaryWeapon_Implementation()
{
	SpawnSecondaryWeapon();
}

bool ABaseCharacter::ServerSpawnSecondaryWeapon_Validate()
{
	return true;
}

void ABaseCharacter::ServerSpawnPistol_Implementation()
{
	SpawnPistol();
}

bool ABaseCharacter::ServerSpawnPistol_Validate()
{
	return true;
}

void ABaseCharacter::ServerSpawnKnife_Implementation()
{
	SpawnKnife();
}

bool ABaseCharacter::ServerSpawnKnife_Validate()
{
	return true;
}

void ABaseCharacter::SpawnInventory()
{
	if (!HasAuthority())
	{
		ServerSpawnInventory();
	}
		SpawnPrimaryWeapon();
		SpawnSecondaryWeapon();
		SpawnPistol();
		SpawnKnife();
}

void ABaseCharacter::SpawnPrimaryWeapon()
{
	if (!HasAuthority())
	{
		ServerSpawnPrimaryWeapon();
	}

	FName WeaponSocket = "WeaponSocket";

		PrimaryWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponLoadout[0]);
		PrimaryWeapon->SetOwner(this);
		PrimaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
		CurrentWeapon = PrimaryWeapon;
		CurrentWeapon->bisEquipped = true;
	
}

void ABaseCharacter::SpawnSecondaryWeapon()
{
	if (!HasAuthority())
	{
		ServerSpawnSecondaryWeapon();
	}
	SecondaryWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponLoadout[1]);
	SecondaryWeapon->SetOwner(this);
	SecondaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SecondaryWeapon->UnequippedSocket);
	SecondaryWeapon->bisEquipped = false;
}

void ABaseCharacter::SpawnPistol()
{
	if (!HasAuthority())
	{
		ServerSpawnPistol();
	}
	Pistol = GetWorld()->SpawnActor<ABaseWeapon>(WeaponLoadout[2]);
	Pistol->SetOwner(this);
	Pistol->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Pistol->UnequippedSocket);
	Pistol->bisEquipped = false;
}

void ABaseCharacter::SpawnKnife()
{
	if (!HasAuthority())
	{
		ServerSpawnKnife();
	}
	Knife = GetWorld()->SpawnActor<ABaseWeapon>(WeaponLoadout[3]);
	Knife->SetOwner(this);
	Knife->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Knife->UnequippedSocket);
	Knife->bisEquipped = false;
}

void ABaseCharacter::WeaponToSpawn()
{
	if (!HasAuthority())
	{
		ServerSpawnWeapon();
	}

	FName WeaponSocket = "WeaponSocket";

	

	
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, FString::Printf(TEXT("WeaponToSpawn")));
}

void ABaseCharacter::UnequipWeapon()
{
	CurrentWeapon->bisEquipped = false;
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->UnequippedSocket);
}



void ABaseCharacter::EquipWeapon()
{
	
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
			WeaponIndex = 0;


			UnequipWeapon();

			//CurrentWeapon->Destroy();
			//WeaponToSpawn();
			EquipWeapon();
		}

		UnequipWeapon();

		//CurrentWeapon->Destroy();

		//WeaponToSpawn();
		EquipWeapon();
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
			WeaponIndex--;

			UnequipWeapon();
			//CurrentWeapon->Destroy();
			//WeaponToSpawn();

			EquipWeapon();
		}
		else
		{
			WeaponIndex = WeaponLoadout.Num() - 1;

			UnequipWeapon();
			//CurrentWeapon->Destroy();
			//WeaponToSpawn();

			EquipWeapon();
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



