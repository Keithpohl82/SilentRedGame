// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Weapons/MasterWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "../SilentRed.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDbugWeaponDrawing(TEXT("DrawDebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for weapons"),
	ECVF_Cheat);


// Sets default values
AMasterWeapon::AMasterWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;


	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	EjectSocketName = "AmmoEject";

	// Sets default values
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

	AmmoPerClip = 0;
	InitialClips = 0;
	BackupAmmo = 0;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	NetUpdateFrequency = 144.0f;
	MinNetUpdateFrequency = 33.0f;

}

void AMasterWeapon::StartReload(bool bFromReplication /*= false*/)
{


	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
	{
		ServerReloadGun();
	}

	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DeterMineWeaponState();

		
			if (GetLocalRole() == ROLE_Authority)
			{
			UE_LOG(LogTemp, Log, TEXT("Reload timer should be called here......"));
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AMasterWeapon::Reload, FMath::Max(0.1f, WeaponConfig.ReloadDuration - 0.1f), false);
			}
			UE_LOG(LogTemp, Log, TEXT("STOP  Reload timer should be called here......"));
			GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &AMasterWeapon::StopReload, WeaponConfig.ReloadDuration, false);

	}
}

void AMasterWeapon::OnEquip(const AMasterWeapon* LastWeapon)
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DeterMineWeaponState();

	if (LastWeapon)
	{
		float Duration = 0.5f;
		EquipStartedTime = GetWorld()->GetTimeSeconds();
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AMasterWeapon::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}
	ABaseCharacter::NotifyEquipWeapon.Broadcast(MyPawn, this);
}

void AMasterWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	DeterMineWeaponState();

	if (MyPawn)
	{
		if (MyPawn->IsLocallyControlled() && CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}
	}
}

void AMasterWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
	bIsEquipped = false;
	StopFire();

	if (bPendingReload)
	{
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}
	ABaseCharacter::NotifyUnEquipWeapon.Broadcast(MyPawn, this);
}

void AMasterWeapon::OnEnterInventory(ABaseCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void AMasterWeapon::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}
}

bool AMasterWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool AMasterWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

EWeaponState::Type AMasterWeapon::GetCurrentState() const
{
	return CurrentState;
}

void AMasterWeapon::SetOwningPawn(class ABaseCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;

		SetOwner(NewOwner);
	}
}

// Called when the game starts or when spawned
void AMasterWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / WeaponConfig.RateOfFire;

	Ammo = AmmoPerClip;
	
}

void AMasterWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetWeaponAttachPoint();

		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecificPawnMesh(true);
			MeshComp->SetHiddenInGame(false);
			MeshComp->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AMasterWeapon::DetachMeshFromPawn()
{

	USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();

	MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	MeshComp->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, MyPawn->UnEquippedSocket);
	MeshComp->SetHiddenInGame(false);
	

}

void AMasterWeapon::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		UGameplayStatics::SpawnEmitterAttached(EjectEffect, MeshComp, EjectSocketName);
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
	Recoil();
}

void AMasterWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, ImpactPoint, ShotDirection.Rotation());

	}
}

void AMasterWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	if (Ammo <= 0)
	{
		UGameplayStatics::SpawnSoundAttached(OutOfAmmoSound, MeshComp, MuzzleSocketName);

	}

	if (Ammo > 0)
	{

		if (bIsShotGun)
		{
			for (int32 i = 0; i < WeaponConfig.ShotgunPellets; i++)
			{
				Ammo--;

				AActor* MyOwner = GetOwner();

				if (MyOwner)
				{
					FVector EyeLocation;
					FRotator EyeRotation;

					APawn* PawnOwner = Cast<APawn>(GetOwner());
					AController* ControllerOwner = PawnOwner->GetController();
					ControllerOwner->GetPlayerViewPoint(EyeLocation, EyeRotation);

					float SpreadMax = 1.2;
					float SpreadMin = -1.1;
					FVector ShotDirection = FMath::VRandCone(EyeRotation.Vector(), 0.15);

					FVector TraceEnd = EyeLocation + (ShotDirection * WeaponConfig.BulletEndPoint);

					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(MyOwner);
					QueryParams.AddIgnoredActor(this);

					QueryParams.bTraceComplex = true;
					QueryParams.bReturnPhysicalMaterial = true;

					FVector TracerEndPoint = TraceEnd;

					EPhysicalSurface SurfaceType = SurfaceType_Default;

					FHitResult Hit;


					if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
					{
						AActor* HitActor = Hit.GetActor();

						SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

						float ActualDamage = BaseDamage;
						if (SurfaceType == SURFACE_FLESHVULNERABLE)
						{
							ActualDamage *= HeadShotBonus;
						}

						UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

						PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

						TracerEndPoint = Hit.ImpactPoint;
					}

					if (DebugWeaponDrawing > 0)
					{
						DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 10.0f, 0, 1.0f);
					}

					PlayFireEffects(TracerEndPoint);

					if (GetLocalRole() == ROLE_Authority)
					{
						HitScanTrace.TraceTo = TracerEndPoint;
						HitScanTrace.SurfaceType = SurfaceType;
					}

					LastFireTime = GetWorld()->TimeSeconds;
				}
			}
		}
		else
		{
			Ammo--;

			AActor* MyOwner = GetOwner();

			if (MyOwner)
			{
				FVector EyeLocation;
				FRotator EyeRotation;

				APawn* PawnOwner = Cast<APawn>(GetOwner());
				AController* ControllerOwner = PawnOwner->GetController();
				ControllerOwner->GetPlayerViewPoint(EyeLocation, EyeRotation);

				FVector ShotDirection = EyeRotation.Vector();

				FVector TraceEnd = EyeLocation + (ShotDirection * WeaponConfig.BulletEndPoint);

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(MyOwner);
				QueryParams.AddIgnoredActor(this);

				QueryParams.bTraceComplex = true;
				QueryParams.bReturnPhysicalMaterial = true;

				FVector TracerEndPoint = TraceEnd;

				EPhysicalSurface SurfaceType = SurfaceType_Default;

				FHitResult Hit;


				if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
				{
					AActor* HitActor = Hit.GetActor();

					SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

					float ActualDamage = BaseDamage;
					if (SurfaceType == SURFACE_FLESHVULNERABLE)
					{
						ActualDamage *= HeadShotBonus;
					}

					UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

					PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

					TracerEndPoint = Hit.ImpactPoint;
				}

				if (DebugWeaponDrawing > 0)
				{
					DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Green, false, 10.0f, 0, 1.0f);
				}

				PlayFireEffects(TracerEndPoint);

				if (GetLocalRole() == ROLE_Authority)
				{
					HitScanTrace.TraceTo = TracerEndPoint;
					HitScanTrace.SurfaceType = SurfaceType;
				}

				LastFireTime = GetWorld()->TimeSeconds;
			}
		}
	}
}

void AMasterWeapon::Recoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = RecoilTimePerShot + TimeBetweenShots;
			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));

		}

	}
}

void AMasterWeapon::ResetRecoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = 0;
			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot);
			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot);
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
		}
	}
}

void AMasterWeapon::DeterMineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload() == false)
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}
		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}
	SetWeaponState(NewState);
}

// Called every frame
void AMasterWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMasterWeapon::StartFire()
{
	if (isAuto)
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AMasterWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	}
	else
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AMasterWeapon::Fire, TimeBetweenShots, false, FirstDelay);

	}
}

void AMasterWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	ResetRecoil();
}

void AMasterWeapon::ServerReloadGun_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("ServerReload called"));

	StartReload(true);
}

bool AMasterWeapon::ServerReloadGun_Validate()
{
	return true;
}

void AMasterWeapon::Reload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerReloadGun();
	}

	if (BackupAmmo >= AmmoPerClip)
	{
		BackupAmmo = BackupAmmo - AmountToReload;
		AmountToReload = AmmoPerClip - Ammo;
		Ammo = AmmoPerClip;
	}
	else
	{
		AmountToReload = BackupAmmo;
		BackupAmmo = 0;
		Ammo = Ammo + AmountToReload;
	}
}

int AMasterWeapon::GetAmmo()
{
	return Ammo;
}

int AMasterWeapon::GetTotalAmmo()
{
	return BackupAmmo;
}

void AMasterWeapon::ServerFire_Implementation()
{
	Fire();
}

bool AMasterWeapon::ServerFire_Validate()
{
	return true;
}

void AMasterWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void AMasterWeapon::OnRep_Reload()
{
	if (bPendingReload)
	{

		StartReload(true);

	}
	else
	{
		StopReload();
	}
}

void AMasterWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}

void AMasterWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DeterMineWeaponState();
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}
}

void AMasterWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;
	CurrentState = NewState;
}

bool AMasterWeapon::CanFire() const
{
	bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
}

bool AMasterWeapon::CanReload() const
{
	bool bCanReload = (!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0);
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
}

USkeletalMeshComponent* AMasterWeapon::GetWeaponMesh() const
{
	return MeshComp;
}

void AMasterWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMasterWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(AMasterWeapon, CurrentAmmoInClip);
	DOREPLIFETIME(AMasterWeapon, CurrentAmmo);
	DOREPLIFETIME(AMasterWeapon, AmountToReload);
	DOREPLIFETIME(AMasterWeapon, Ammo);
	DOREPLIFETIME(AMasterWeapon, BackupAmmo);
	DOREPLIFETIME(AMasterWeapon, InitialClips);
	DOREPLIFETIME(AMasterWeapon, AmmoPerClip);
	DOREPLIFETIME(AMasterWeapon, bPendingReload);
	DOREPLIFETIME(AMasterWeapon, MyPawn);
}