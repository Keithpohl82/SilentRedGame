// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Weapons/BaseWeapon.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SilentRed/SilentRed.h"
#include "TimerManager.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Core/BasePlayerState.h"


static int32 DebugWeaponDawing = 0;

FAutoConsoleVariableRef CVARDebugWeaponDrawing (
TEXT("Weapon.DebugWeapons"), 
DebugWeaponDawing, 
TEXT("Draws Debug lines for weapons"), 
ECVF_Cheat);


// Sets default values
ABaseWeapon::ABaseWeapon()
{
 
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MeshComp->SetOnlyOwnerSee(true);
	


	WeaponRange = 10000;
	HeadShotMultiplyer = 2.5;
	RateOfFire = 600;
	ClipAmmo = DefaultClipAmmo;
	MaxAmmo = DefaultMaxAmmo;

	SetReplicates(true);

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	NetUpdateFrequency = 144.0f;
	MinNetUpdateFrequency = 33.0f;

}



///////////For replicating information to players/////////

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, WeaponTrace);
	DOREPLIFETIME(ABaseWeapon, ClipAmmo);
	DOREPLIFETIME(ABaseWeapon, DefaultClipAmmo);
	DOREPLIFETIME(ABaseWeapon, MaxAmmo);
	DOREPLIFETIME(ABaseWeapon, DefaultMaxAmmo);
}






void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ABaseWeapon::Fire()
{

	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	AActor* MyOwner = GetOwner();
	
	
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * WeaponRange);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if(ClipAmmo > 0)
		{
		 ClipAmmo--;
		

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamageAmount;

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= HeadShotMultiplyer;
			}

			

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayWeaponEffects(TracerEndPoint);

		if (GetLocalRole() == ROLE_Authority)
		{
			WeaponTrace.TraceTo = TracerEndPoint; 
			WeaponTrace.SurfaceType = SurfaceType;
		}
		LastFireTime = GetWorld()->TimeSeconds;
		}

	}
}

void ABaseWeapon::ReloadWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerReloadWeapon();
	}

	if (MaxAmmo == 0)
	{
		return;
	}
	
	int32 AmmoToReload;

	AmmoToReload = DefaultClipAmmo - ClipAmmo;

	if (ClipAmmo + MaxAmmo < DefaultClipAmmo)
	{
		ClipAmmo = ClipAmmo + MaxAmmo;
		MaxAmmo = 0;
	}
	else
	{
		//setter for MaxAmmo
		MaxAmmo = FMath::Clamp(MaxAmmo - AmmoToReload, 0, DefaultMaxAmmo);
		ClipAmmo = DefaultClipAmmo;
	}
}

void ABaseWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ABaseWeapon::ServerFire_Validate()
{
	return true;
}

void ABaseWeapon::ServerReloadWeapon_Implementation()
{
	ReloadWeapon();
}

bool ABaseWeapon::ServerReloadWeapon_Validate()
{
	return true;
}

void ABaseWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ABaseWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ABaseWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	RecoilReset();
}

void ABaseWeapon::RecoilReset()
{
	GetWorldTimerManager().SetTimer(TimerHandle_RecoilReset, this, &ABaseWeapon::ResetRecoil, RecoilRecoveryTime, false);
}

void ABaseWeapon::PlayWeaponEffects(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
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

void ABaseWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
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
	}
}

//Recoil Sets which point on the curve to call. Adjust curves to change recoil
void ABaseWeapon::Recoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = RecoilTimePerShot + TimeBetweenShots;
			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
		}
	}
}

//Resets the recoil to starting point of the curve
void ABaseWeapon::ResetRecoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = DefaultRecoilTimePerShot;
			float NewPitch = 0;
			float NewYaw = 0;
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
		}
	}
}

void ABaseWeapon::OnRep_WeaponTrace()
{
	PlayWeaponEffects(WeaponTrace.TraceTo);

	PlayImpactEffects(WeaponTrace.SurfaceType, WeaponTrace.TraceTo);
}
