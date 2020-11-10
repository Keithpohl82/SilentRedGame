// Fill out your copyright notice in the Description page of Project Settings.


#include "SilentRed/Public/Steam/SteamFriendsFunctionLib.h"

#include "Memory.h"
#include "Engine/Texture2D.h"

// This will lead to some warnings in the log. Don't worry.
#include <steam/steam_api.h>
#include "Engine/Texture.h"

UTexture2D* USteamFriendsFunctionLib::GetSteamFriendAvatar(const FSteamFriend SteamFriend, ESFAsyncResultSwitch& Result, ESFAvatarSize AvatarSize /*= ESFAvatarSize::SF_MEDIUM*/)
{
	if (!SteamFriend.UniqueNetID.IsValid())
	{
		Result = ESFAsyncResultSwitch::OnFailure;
		return nullptr;
	}

	// Size of the Texture
	uint32 Height = 0;
	uint32 Width = 0;

	if (SteamAPI_Init())
	{
		// Convert the ID to a uint64 pointer
		uint64 ID = *((uint64*)SteamFriend.UniqueNetID->GetBytes());

		// Set the Avatar Size Number
		int AvatarSizeNumber = 0;

		switch (AvatarSize)
		{
		case ESFAvatarSize::SF_SMALL:
			AvatarSizeNumber = SteamFriends()->GetSmallFriendAvatar(ID);
			break;
		case ESFAvatarSize::SF_MEDIUM:
			AvatarSizeNumber = SteamFriends()->GetMediumFriendAvatar(ID);
			break;
		case ESFAvatarSize::SF_LARGE:
			AvatarSizeNumber = SteamFriends()->GetLargeFriendAvatar(ID);
			break;
		default:
			break;
		}

		// Still Loading...
		if (AvatarSizeNumber == -1)
		{
			Result = ESFAsyncResultSwitch::AsyncLoading;
			return nullptr;
		}

		// Retrieve the actual Image Size, because we need that to reserve enough memory
		SteamUtils()->GetImageSize(AvatarSizeNumber, &Width, &Height);

		if (Height > 0 && Width > 0)
		{
			// Buffer for RGBA Data (4 * cause of R G B A)
			uint8* AvatarRGBA = new uint8[Height * Width * 4];

			SteamUtils()->GetImageRGBA(AvatarSizeNumber, AvatarRGBA, Height * Width * 4 * sizeof(char));

			// Create a Placeholder Texture to fill the Buffer in
			UTexture2D* AvatarTexture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

			// Lock the BulkData and MemCopy
			uint8* MipData = (uint8*)AvatarTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)AvatarRGBA, Height * Width * 4);
			AvatarTexture->PlatformData->Mips[0].BulkData.Unlock();

			// Clear up the memory!
			delete[] AvatarRGBA;

			// Update the Texture2D Settings
			AvatarTexture->PlatformData->SetNumSlices(1);
			AvatarTexture->NeverStream = true;
			AvatarTexture->UpdateResource();

			Result = ESFAsyncResultSwitch::OnSuccess;

			return AvatarTexture;
		}
	}

	Result = ESFAsyncResultSwitch::OnFailure;
	return nullptr;
}

void USteamFriendsFunctionLib::GetSteamFriendGame(const FSteamFriend SteamFriend, ESFAsyncResultSwitch& Result, FString& GameName)
{
	if (!SteamFriend.UniqueNetID.IsValid())
	{
		Result = ESFAsyncResultSwitch::OnFailure;
		return;
	}

	if (SteamAPI_Init())
	{
		// Convert the ID to a uint64 pointer
		uint64 ID = *((uint64*)SteamFriend.UniqueNetID->GetBytes());

		// Get the GameInfo from the Player via ID
		FriendGameInfo_t GameInfo;
		bool bIsInGame = SteamFriends()->GetFriendGamePlayed(ID, &GameInfo);

		// If InGame and the GameID is actually valid, try to get the AppName
		if (bIsInGame && GameInfo.m_gameID.IsValid())
		{
			char NameBuffer[512];
			int NameLength = SteamAppList()->GetAppName(GameInfo.m_gameID.AppID(), NameBuffer, 512);

			if (NameLength != -1) // Invalid
			{
				GameName = FString(UTF8_TO_TCHAR(NameBuffer));
			}

			Result = ESFAsyncResultSwitch::OnSuccess;
			return;
		}

	}

	Result = ESFAsyncResultSwitch::OnFailure;
}
