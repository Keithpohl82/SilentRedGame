// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Weapons/BasePistol.h"
#include "SilentRed/Public/Weapons/BaseWeapon.h"
#include "SilentRed/SilentRed.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"



static int32 DebugWeaponDawing = 0;

ABasePistol::ABasePistol()
{
	ClipAmmo = DefaultClipAmmo;
	MaxAmmo = DefaultMaxAmmo;

	SetReplicates(true);

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	NetUpdateFrequency = 144.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ABasePistol::Fire()
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
		if (ClipAmmo > 0)
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
