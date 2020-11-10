// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Additional Includes
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystemUtils.h"
#include "SilentRed/Public/Steam/SteamFriendsFunctionLib.h"

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "GetSteamFriendsCallProxyBase.generated.h"


/** Delegate to Broadcast the results. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetSteamFriendsDelegate, const TArray<FSteamFriend>&, Results);


UCLASS()
class SILENTRED_API UGetSteamFriendsCallProxyBase : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

		/// VARIABLES ///

private:

	/** Pointer to the PlayerController that tries to retrieve the Steam Friends. */
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	/** Pointer to our World Context Object */
	UObject* WorldContextObject;

	/** Delegate for the async Steam Friend Request. */
	FOnReadFriendsListComplete OnReadFriendsCompleteDelegate;

public:

	/** Delegate (BP Node Exec) called when we successfully retrieved the Steam Friends. */
	UPROPERTY(BlueprintAssignable)
		FGetSteamFriendsDelegate OnSuccess;

	/** Delegate (BP Node Exec) called when we failed to retrieve the Steam Friends. */
	UPROPERTY(BlueprintAssignable)
		FGetSteamFriendsDelegate OnFailure;

	/// FUNCTIONS ///

public:

	/**
	*	Static function, used internally to create the Proxy and fill in the needed variables.
	*
	*	@param		InWorldContextObject		World Context Object, in case we need the World.
	*	@param		InPlayerController			PlayerController, which tries to get Steam Friends.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"), Category = "Steam|Friends")
		static UGetSteamFriendsCallProxyBase* GetSteamFriends(UObject* InWorldContextObject, class APlayerController* InPlayerController);

	/** Internal Callback when Friend List Request Completes */
	void OnGetSteamFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	/** START - UOnlineBlueprintCallProxyBase Interface */
	virtual void Activate() override;
	/** END - UOnlineBlueprintCallProxyBase Interface */
};
