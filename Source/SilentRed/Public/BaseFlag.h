// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseFlag.generated.h"


class ABaseCharacter;
class UStaticMeshComponent;
class ACTF_CapturePoint;


UCLASS()
class SILENTRED_API ABaseFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseFlag();

	void AttachFlagToPlayer(ABaseCharacter* Player);//function to attach flag to player

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttachFlagToPlayer(ABaseCharacter* Player);//server version

	void Detach();//detach flag once destination is reached

	UFUNCTION(Server, Reliable, WithValidation)//server version
	void ServerDetach();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginPlay();//server version

	UPROPERTY(EditDefaultsOnly, Category = "Flag")
	UStaticMeshComponent* MeshCompFlag;




	UPROPERTY(Replicated)
	ACTF_CapturePoint* Destination;//object which shows where the destination is

	void OnReachDest();//function that handles the event when destination is reached


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated)
	int32 TeamNum;//team number

};