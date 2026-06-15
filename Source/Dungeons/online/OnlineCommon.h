#pragma once

#ifdef _MSC_VER
#pragma warning(push)	//incomplete switch
#pragma warning(disable:4062)
#endif
#include "CoreMinimal.h"
#include "OnlinePartyInterface.h"
#include "OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#ifdef _MSC_VER
#pragma warning(pop)	//incomplete switch
#endif

//D11.TT - This needs to match the PlayerState::SessionName ("GameSession"). Some thing seem to use this FName and some things use the PlayerState one. This is a quick fix
const FName DungeonsGameSessionName(TEXT("GameSession"));

const int PlatformMaxPlayers = 4; //4 player even on Switch!

#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
static constexpr const wchar_t* ConnectionCheckURL = TEXT("api.minecraftservices.com");
#endif
