// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Steam/GetSteamFriendsCallProxyBase.h"
#include "Engine/LocalPlayer.h"


UGetSteamFriendsCallProxyBase::UGetSteamFriendsCallProxyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	// Bind Function to Delegate
	, OnReadFriendsCompleteDelegate(FOnReadFriendsListComplete::CreateUObject(this, &UGetSteamFriendsCallProxyBase::OnGetSteamFriendsComplete))
{
}

UGetSteamFriendsCallProxyBase* UGetSteamFriendsCallProxyBase::GetSteamFriends(UObject* InWorldContextObject, class APlayerController* InPlayerController)
{
	// Create a new Proxy and fill in the passed variables.
	UGetSteamFriendsCallProxyBase* Proxy = NewObject<UGetSteamFriendsCallProxyBase>();
	Proxy->WorldContextObject = InWorldContextObject;
	Proxy->PlayerControllerWeakPtr = InPlayerController;

	return Proxy;
}

void UGetSteamFriendsCallProxyBase::OnGetSteamFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	TArray<FSteamFriend> SteamFriends;

	if (bWasSuccessful)
	{
		IOnlineFriendsPtr FriendInterface = Online::GetFriendsInterface();

		if (FriendInterface.IsValid())
		{
			// Now that we are sure that everything worked, try to actually get the FriendList
			TArray<TSharedRef<FOnlineFriend>> FriendList;
			FriendInterface->GetFriendsList(LocalUserNum, ListName, FriendList);

			// Iterate over the FriendList and fill in all Data into our Dummy Struct
			for (TSharedRef<FOnlineFriend> Friend : FriendList)
			{
				// Get Presence State
				FOnlineUserPresence Presence = Friend->GetPresence();

				// Dummy Array Entry
				FSteamFriend SteamFriend;

				SteamFriend.DisplayName = Friend->GetDisplayName();
				SteamFriend.RealName = Friend->GetRealName();
				SteamFriend.StatusString = Presence.Status.StatusStr;
				SteamFriend.bIsOnline = Presence.bIsOnline;
				SteamFriend.bIsPlaying = Presence.bIsPlaying;
				SteamFriend.bIsPlayingSameGame = Presence.bIsPlayingThisGame;
				SteamFriend.bIsJoinable = Presence.bIsJoinable;
				SteamFriend.bHasVoiceSupport = Presence.bHasVoiceSupport;
				SteamFriend.PresenceState = (EOnlinePresenceState::Type)(int32)Presence.Status.State;
				SteamFriend.UniqueNetID = Friend->GetUserId();

				SteamFriends.Add(SteamFriend);
			}

			OnSuccess.Broadcast(SteamFriends);
		}
	}

	OnFailure.Broadcast(SteamFriends);
}

void UGetSteamFriendsCallProxyBase::Activate()
{
	if (PlayerControllerWeakPtr.IsValid())
	{
		IOnlineFriendsPtr FriendInterface = Online::GetFriendsInterface();

		if (FriendInterface.IsValid())
		{
			const ULocalPlayer* LocalPlayer = PlayerControllerWeakPtr->GetLocalPlayer();

			// Request the FriendList from the FriendInterface.
			// We want the Default FriendList (All People) here, there are more settings you can try, e.g. "Online" or "InGame"
			FriendInterface->ReadFriendsList(LocalPlayer->GetControllerId(), EFriendsLists::ToString(EFriendsLists::Default), OnReadFriendsCompleteDelegate);
		}
	}

	TArray<FSteamFriend> SteamFriends;
	OnFailure.Broadcast(SteamFriends);
}
