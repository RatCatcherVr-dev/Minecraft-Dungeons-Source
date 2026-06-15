#include "Dungeons.h"
#include "MinecraftAPIConsoleCommands.h"
#include "DungeonsGameInstance.h"
#include "sessions/OnlineUtil.h"
#include "MinecraftAPI.h"
#include "AuthCommon.h"
#include "Util/StringUtil.h"

static void Authenticate(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const auto localUserNum = 0;
	const auto onlineIdentity = online::getIdentityInterface();	
	const auto xuid = onlineIdentity->GetUniquePlayerId(localUserNum);
	onlineIdentity->GetAuthTokenAsync("playfab_key", [world, xuid](FString token, FString issuerId) {
		online::getIdentityInterface()->GetAuthTokenAsync("ServiceToken", [world, xuid, token](FString platformtoken, FString issuerId) {
			auto minecraftApi = world->GetGameInstance<UDungeonsGameInstance>()->GetMinecraftAPI();
			minecraftApi->Authenticate(XAuthData(xuid->ToString(), token, platformtoken, online::GetMinecraftAPINamespace()));
		});
	});
}

static const FAutoConsoleCommand AuthenticateAgainstMinecraftAPI(TEXT("Dungeons.MinecraftAPI.Authenticate")
	, TEXT("Authenticate against Minecraft API using the currently signed in XBL user")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&Authenticate)
	, ECVF_Cheat);
