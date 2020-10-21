// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseFlag.h"
#include "CTF_CapturePoint.h"
#include "SilentRed/Public/Characters/BaseCharacter.h"
#include "SilentRed/Public/Components/HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "SilentRed/Public/Core/CTFGameMode.h"
#include "SilentRed/Public/Core/MasterGameMode.h"
#include "SilentRed/Public/Core/MasterGameState.h"
#include "SilentRed/Public/Core/BasePlayerState.h"
#include "SilentRed/Public/Core/MasterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "SilentRed/SilentRed.h"

// Sets default values
ABaseFlag::ABaseFlag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshCompFlag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flag Mesh"));

	SetReplicates(true);
}
void ABaseFlag::AttachFlagToPlayer(ABaseCharacter* Player)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Player)
		{
			FName Socket = Player->FlagAttachSocketName;
			this->AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
			SetOwner(Player);
		}
	}
	else
	{
		ServerAttachFlagToPlayer(Player);
	}
}

void ABaseFlag::ServerAttachFlagToPlayer_Implementation(ABaseCharacter* Player)
{
	AttachFlagToPlayer(Player);
}

bool ABaseFlag::ServerAttachFlagToPlayer_Validate(ABaseCharacter* Player)
{
	return true;
}

void ABaseFlag::Detach()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detached..."));
		TArray<AActor*> FlagPickupLocations;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTF_CapturePoint::StaticClass(), FlagPickupLocations);

		for (int i = 0; i < FlagPickupLocations.Num(); i++)
		{
			ACTF_CapturePoint* Loc = Cast<ACTF_CapturePoint>(FlagPickupLocations[i]);
			if (Loc)
			{
				if (Loc->GetTeamNum() == TeamNum)
				{
					Destroy();
					DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					Loc->Respawn();
					return;
				}
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("couldn't find respawn loc"));

	}
	else
	{
		ServerDetach();
	}
}

void ABaseFlag::ServerDetach_Implementation()
{
	Detach();
}

bool ABaseFlag::ServerDetach_Validate()
{
	return true;
}

// Called when the game starts or when spawned
void ABaseFlag::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> FlagPickupLocations;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTF_CapturePoint::StaticClass(), FlagPickupLocations);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("START"));


		for (int i = 0; i < FlagPickupLocations.Num(); i++)
		{
			ACTF_CapturePoint* Loc = Cast<ACTF_CapturePoint>(FlagPickupLocations[i]);
			if (Loc)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::SanitizeFloat(Loc->GetDistanceTo(this)));

				if (Loc->GetDistanceTo(this) > 0.0f)//(Loc->GetTeamNum() != TeamNum)
				{
					Destination = Loc;
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::FromInt(TeamNum));
					//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::FromInt(Loc->GetTeamNum()));

				}
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("END"));

	}
	else
	{
		ServerBeginPlay();
	}
}

void ABaseFlag::ServerBeginPlay_Implementation()
{
	BeginPlay();
}

bool ABaseFlag::ServerBeginPlay_Validate()
{
	return true;
}

void ABaseFlag::OnReachDest()
{

		ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetOwner());
		ABasePlayerState* PlayerState = Cast<ABasePlayerState>(PlayerCharacter->GetPlayerState());
		ACTFGameMode* GM = Cast<ACTFGameMode>(GetWorld()->GetAuthGameMode());


	if (GetLocalRole() == ROLE_Authority)
	{
		Detach();
		
		

		if (GM)
		{
			if (TeamNum == 0)
			{
				GM->FlagCapture(1);
			}
			else
			{
				GM->FlagCapture(0);
			}
		}
	}
}

// Called every frame
void ABaseFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	// int32 PlayersHealth = FlagOwner->GetHealth();

	if (GetLocalRole() == ROLE_Authority)
	{
		ABaseCharacter* FlagOwner = Cast<ABaseCharacter>(GetOwner());
		if (FlagOwner)
		{
			if (Destination->GetDistanceTo(FlagOwner) <= 200.f)
			{
				OnReachDest();
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("dest reached"));

				DrawDebugSphere(
					GetWorld(),
					Destination->GetActorLocation(),
					240,
					32,
					FColor(255, 0, 0)
				);
			}
				
			UHealthComponent* OwnerHealth = Cast<UHealthComponent>(FlagOwner->GetComponentByClass(UHealthComponent::StaticClass()));
			if (ensure(OwnerHealth))
			{
				if (OwnerHealth->GetHealthAmount() <= 0)
				{
					Detach();

					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("no health"));

				}
			}
		}
	}

}

void ABaseFlag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseFlag, TeamNum);
	DOREPLIFETIME(ABaseFlag, Destination);
}