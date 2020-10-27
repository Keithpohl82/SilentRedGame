// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTF_CapturePoint.generated.h"


class USphereComponent;
class UDecalComponent;
class ABaseCharacter;
class ABaseFlag;
UCLASS()
class SILENTRED_API ACTF_CapturePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACTF_CapturePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UDecalComponent* DecalComp;

	UPROPERTY(EditAnywhere, Category = "Flag")
		TSubclassOf<ABaseFlag> FlagClass;

	UPROPERTY(EditDefaultsOnly, Category = "Flag")
		float CoolDownDuration;

	FTimerHandle RespawnTimer;


	UPROPERTY(Replicated)
	ABaseFlag* FlagInstance;

	UPROPERTY(Replicated, EditAnywhere, Category = "Flag")
	int32 TeamNum;

	void GiveFlag(ABaseCharacter* Target);

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* FlagStolen;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void Respawn();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRespawn();

	int32 GetTeamNum();
	

};
