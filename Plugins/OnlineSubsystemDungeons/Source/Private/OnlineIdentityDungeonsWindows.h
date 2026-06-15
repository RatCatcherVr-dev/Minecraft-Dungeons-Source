#if PLATFORM_WINDOWS
#pragma once

#include "OnlineSubsystemDungeons.h"
#include "OnlineIdentityDungeons.h"

#if defined(PLATFORM_WINDOWS) && !defined(STEAM_BUILD) && (!defined(GDK_API_ENABLED) || GDK_API_ENABLED == 0)
#define XAL_STORAGE 1
#else
#define XAL_STORAGE 0
#endif



#if XAL_STORAGE
#include "XalStorage.h"
#endif

class FOnlineIdentityDungeonsWindows : public FOnlineIdentityDungeons
{

public:
	FOnlineIdentityDungeonsWindows(class FOnlineSubsystemDungeons* InSubsystem);
	virtual ~FOnlineIdentityDungeonsWindows() = default;

private:
	void InitializePlatform();

#if XAL_STORAGE
	std::unique_ptr<XalStorage> xalStorage;
#endif
};

typedef TSharedPtr<FOnlineIdentityDungeonsWindows, ESPMode::ThreadSafe> FOnlineIdentityDungeonsWindowsPtr;

#endif
