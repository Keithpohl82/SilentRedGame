// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerHealthComp.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FOnHealthChangedSignature, UPlayerHealthComp*, HealthComp, float, Health, float, Armor, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SILENTRED_API UPlayerHealthComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerHealthComp();

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = Health)
	float Health;

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category = Health)
	float Armor;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	float DefaultHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
		float DefaultArmor;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float ArmorDamageReduction;

	float ArmorDamage;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable)
	float GetHealthAmount();
		
};
