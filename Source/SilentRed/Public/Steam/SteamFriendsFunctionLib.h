// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interfaces/OnlinePresenceInterface.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamFriendsFunctionLib.generated.h"


/**
*	Struct defining a SteamFriend, which can be accessed in Blueprints.
*/
USTRUCT(BlueprintType)
struct FSteamFriend
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		FString RealName;

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		FString StatusString;

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		bool bIsOnline;

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		bool bIsPlaying;

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		bool bIsPlayingSameGame;

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		bool bIsJoinable;

	UPROPERTY(BlueprintReadOnly, Category = "Steam|Friends")
		bool bHasVoiceSupport;

	EOnlinePresenceState::Type PresenceState;

	TSharedPtr<const FUniqueNetId> UniqueNetID;
};

/**
*	Enum used for the Execs of the Static Functions.
*/
UENUM()
enum class ESFAsyncResultSwitch : uint8
{
	OnSuccess,
	AsyncLoading,
	OnFailure
};

/**
*	Enum use for requesting a specific Avatar size.
*/
UENUM(Blueprintable)
enum class ESFAvatarSize : uint8
{
	DEFAULT = 0,
	SF_SMALL = 1	UMETA(DisplayName = "Small Avatar"),
	SF_MEDIUM = 2	UMETA(DisplayName = "Medium Avatar"),
	SF_LARGE = 3	UMETA(DisplayName = "Large Avatar")
};
/**
 * 
 */
UCLASS()
class SILENTRED_API USteamFriendsFunctionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
		/// FUNCTIONS ///

public:

	/**
	*	Gets the Avatar of the specified Steam Friend.
	*
	*	@param		SteamFriend		Steam Friend we want the Avatar from
	*	@param		Result			Result Enum used for Execs
	*	@param		AvatarSize		Size of the Avatar we want (default: Medium)
	*/
	UFUNCTION(BlueprintCallable, Category = "Steam|Friends", meta = (ExpandEnumAsExecs = "Result"))
		static class UTexture2D* GetSteamFriendAvatar(const FSteamFriend SteamFriend, ESFAsyncResultSwitch& Result, ESFAvatarSize AvatarSize = ESFAvatarSize::SF_MEDIUM);

	/**
	*	Gets the Game of the specified Steam Friend.
	*
	*	@param		SteamFriend		Steam Friend we want the Game from
	*	@param		Result			Result Enum used for Execs
	*	@param		GameName		Name of the Game (if found)
	*/
	UFUNCTION(BlueprintCallable, Category = "Steam|Friends", meta = (ExpandEnumAsExecs = "Result"))
		static void GetSteamFriendGame(const FSteamFriend SteamFriend, ESFAsyncResultSwitch& Result, FString& GameName);

};
