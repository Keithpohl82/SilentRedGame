// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/MasterGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Weapons/MasterWeapon.h"
#include"SilentRed/SilentRed.h"
#include "SilentRed/Public/Components/PlayerHealthComp.h"
#include "SilentRed/Public/Components/WeaponInventoryComponent.h"


 FOnBaseCharacterEquipWeapon ABaseCharacter::NotifyEquipWeapon;
 FOnBaseCharacteUnEquipWeapon ABaseCharacter::NotifyUnEquipWeapon;



void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimerForNextTick(this, &ABaseCharacter::SpawnDefaultInventory);
}

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

	bIsFlagOwner = false;

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, CurrentWeapon);
	DOREPLIFETIME(ABaseCharacter, PlayerSkins);
	DOREPLIFETIME(ABaseCharacter, TeamNum);
	DOREPLIFETIME(ABaseCharacter, WeaponLoadout);
	DOREPLIFETIME(ABaseCharacter, WeaponIndex);
	DOREPLIFETIME(ABaseCharacter, DeathAnim);
	DOREPLIFETIME(ABaseCharacter, bIsFlagOwner);
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

}

FName ABaseCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachSocketName;
}

bool ABaseCharacter::CanFire()
{
	return bDied;
}

bool ABaseCharacter::CanReload()
{
	return true;
}

USkeletalMeshComponent* ABaseCharacter::GetSpecificPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? GetMesh() : GetMesh();
}

USkeletalMeshComponent* ABaseCharacter::GetPawnMesh() const
{
	return GetMesh();
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
		CurrentWeapon->Reload();
	}
}



void ABaseCharacter::ServerEquipWeapon_Implementation(class AMasterWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

bool ABaseCharacter::ServerEquipWeapon_Validate(class AMasterWeapon* NewWeapon)
{
	return true;
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


void ABaseCharacter::SetCurrentWeapon(class AMasterWeapon* NewWeapon, class AMasterWeapon* LastWeapon /*= NULL*/)
{
	AMasterWeapon* LocalLastWeapon = nullptr;
	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	//UnEquip previous weapon
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	//Equip a new weapon
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);

		NewWeapon->OnEquip(LastWeapon);
	}
}


void ABaseCharacter::EquipWeapon(class AMasterWeapon* Weapon)
{
	if (Weapon)
	{

		SetCurrentWeapon(Weapon, CurrentWeapon);

		if (GetLocalRole() < ROLE_Authority)
		{
			ServerEquipWeapon(Weapon);

		}
		
	}
}

void ABaseCharacter::OnRep_CurrentWeapon(class AMasterWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ABaseCharacter::Destroyed()
{
	Super::Destroyed();
	DestroyInventory();
}

int32 ABaseCharacter::GetInventoryCount() const
{
	return Inventory.Num();
}

class AMasterWeapon* ABaseCharacter::GetInventoryWeapon(int32 index) const
{
	return Inventory[index];
}

void ABaseCharacter::AddWeapon(class AMasterWeapon* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void ABaseCharacter::RemoveWeapon(class AMasterWeapon* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

class AMasterWeapon* ABaseCharacter::FindWeapon(TSubclassOf<class AMasterWeapon> WeaponClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return NULL;
}

void ABaseCharacter::NextWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerNextWeapon();
	}

	AMasterPlayerController* MyPC = Cast<AMasterPlayerController>(Controller);
	if (MyPC)
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AMasterWeapon* NextWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
			EquipWeapon(NextWeapon);
		}
	}
}

void ABaseCharacter::PreviousWeapon()
{

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerPreviousWeapon();
	}

	AMasterPlayerController* MyPC = Cast<AMasterPlayerController>(Controller);
	if (MyPC)
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AMasterWeapon* PrevWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
			EquipWeapon(PrevWeapon);
		}
	}
}

void ABaseCharacter::SpawnDefaultInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClass = WeaponLoadout.Num();
	for (int32 i = 0; i < NumWeaponClass; i++)
	{
		if (WeaponLoadout[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AMasterWeapon* NewWeapon = GetWorld()->SpawnActor<AMasterWeapon>(WeaponLoadout[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void ABaseCharacter::DestroyInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AMasterWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
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
		//PC->Deaths++;
		GetMesh()->PlayAnimation(DeathAnim, false);

		DetachFromControllerPendingDestroy();

		//DestroyInventory();

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



