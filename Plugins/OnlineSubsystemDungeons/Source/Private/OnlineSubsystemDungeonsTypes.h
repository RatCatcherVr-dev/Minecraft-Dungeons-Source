#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemTypes.h"
#include "xsapiServicesInclude.h"
#include <inttypes.h>
#include <string>

#ifndef DUNGEONSPLATFORM
#define DUNGEONSPLATFORM FName(TEXT("DUNGEONS"))
#endif

using FUniqueNetIdDungeonsRef = TSharedRef<const class FUniqueNetIdDungeons>;

class FUniqueNetIdDungeons : public FUniqueNetId
{

public:

	static FUniqueNetIdDungeonsRef Create(uint64 XUID)
	{
		return MakeShareable(new FUniqueNetIdDungeons(XUID));
	}

	static FUniqueNetIdDungeonsRef Create(const FUniqueNetId& InUniqueNetId)
	{
		return StaticCastSharedRef<const FUniqueNetIdDungeons>(InUniqueNetId.AsShared());
	}

	uint64 XUID;

	FUniqueNetIdDungeons()
		: XUID(0)
	{}

	virtual ~FUniqueNetIdDungeons() = default;
	FUniqueNetIdDungeons(const FUniqueNetIdDungeons&) = default;
	FUniqueNetIdDungeons(FUniqueNetIdDungeons&&) = default;
	FUniqueNetIdDungeons& operator=(const FUniqueNetIdDungeons&) = default;
	FUniqueNetIdDungeons& operator=(FUniqueNetIdDungeons&&) = default;

	explicit FUniqueNetIdDungeons(uint64 InXUID)
		: XUID(InXUID)
	{}

	explicit FUniqueNetIdDungeons(const FUniqueNetId& InUniqueNetId) :
		XUID(*(uint64*)InUniqueNetId.GetBytes())
	{}

	virtual FName GetType() const override
	{
		return DUNGEONSPLATFORM;
	}

	virtual const uint8* GetBytes() const override
	{
		return (uint8*)&XUID;
	}
	virtual int32 GetSize() const override
	{
		return sizeof(uint64);
	}
	virtual bool IsValid() const override
	{
		return XUID != 0;
	}
	virtual FString ToString() const override
	{
#if PLATFORM_SWITCH || PLATFORM_PS4
		return FString::Printf(TEXT("%llu"), XUID);
#else
		return FString::Printf(TEXT("%" PRIu64), XUID);
#endif
	}
	virtual FString ToDebugString() const override
	{
#if PLATFORM_SWITCH || PLATFORM_PS4
		return FString::Printf(TEXT("%llu"), XUID);
#else
		return FString::Printf(TEXT("%" PRIu64), XUID);
#endif
	}

	/** Needed for TMap::GetTypeHash() */
	friend uint32 GetTypeHash(const FUniqueNetIdDungeons& A)
	{
		return (uint32)(A.XUID) + ((uint32)((A.XUID) >> 32) * 23);
	}

	friend FArchive& operator<<(FArchive& Ar, FUniqueNetIdDungeons& UserId)
	{
		return Ar << UserId.XUID;
	}

};

class FOnlineSessionInfoDungeons : public FOnlineSessionInfo 
{
public:
	explicit FOnlineSessionInfoDungeons();
	explicit FOnlineSessionInfoDungeons(const string_t& SessionId);
	explicit FOnlineSessionInfoDungeons(const FString& SessionId);
	FOnlineSessionInfoDungeons(FUniqueNetIdString SessionId);

	const FUniqueNetIdString& GetSessionIdAsUniqueNetIdString() const;
	void SetSessionId(const FUniqueNetIdString& SessionId);

	const FUniqueNetId& GetSessionId() const override;
	const uint8* GetBytes() const override;
	int32 GetSize() const override;
	bool IsValid() const override;
	FString ToString() const override;
	FString ToDebugString() const override;
	
	FUniqueNetIdString SessionId;
	TSharedPtr<class FInternetAddr> HostAddr;
	FString HandleId;
};