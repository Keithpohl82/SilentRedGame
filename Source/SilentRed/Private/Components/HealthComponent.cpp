// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	SetIsReplicatedByDefault(true);
	DefaultHealth = 100;
	DefaultArmor = 100;
}


void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, Armor);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
		}
	}
	Health = DefaultHealth;
	Armor = DefaultArmor;
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
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
}

float UHealthComponent::GetHealthAmount()
{
	return Health / DefaultHealth;
}

