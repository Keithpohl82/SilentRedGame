// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Components/PlayerHealthComp.h"
#include "SilentRed/Public/Core/MasterGameMode.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"



void UPlayerHealthComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerHealthComp, Health);
	DOREPLIFETIME(UPlayerHealthComp, Armor);
}

// Sets default values for this component's properties
UPlayerHealthComp::UPlayerHealthComp()
{
	
	
	SetIsReplicatedByDefault(true);
	DefaultHealth = 100;
	DefaultArmor = 100;
}

// Called when the game starts
void UPlayerHealthComp::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UPlayerHealthComp::HandleTakeAnyDamage);
		}
	}
	Health = DefaultHealth;
	Armor = DefaultArmor;
}

void UPlayerHealthComp::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	ArmorDamage = ArmorDamageReduction * Damage;

	if (Armor > ArmorDamage)
	{
		Armor -= ArmorDamage;
	}
	else
	{
		Health -= Damage - Armor;
		Armor = 0.0f;
	}

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s, Armor Changed: %s"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(Armor));

	OnHealthChanged.Broadcast(this, Health, Armor, Damage, DamageType, InstigatedBy, DamageCauser);

	if (Health <= 0)
	{
		AMasterGameMode* GM = Cast<AMasterGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			AMasterPlayerController* KillerPC = Cast<AMasterPlayerController>(InstigatedBy);
			
			KillerPC->Kills++;
			KillerPC->Points++;
			
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);

			UE_LOG(LogTemp, Log, TEXT(" %s, Killed %s"), InstigatedBy, GetOwner());
		}
	}
}

float UPlayerHealthComp::GetHealthAmount()
{
	return Health / DefaultHealth;
}


