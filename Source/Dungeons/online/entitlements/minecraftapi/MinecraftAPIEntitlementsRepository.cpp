#include "Dungeons.h"
#include "MinecraftAPIEntitlementsRepository.h"
#include "IDungeonsAPIClient.h"
#include "MinecraftAPIEntitlements.h"

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UMinecraftAPIEntitlementsRepository::RequestEntitlements() {
	ANTICHEAT_VIRT_BEGIN
	if (const auto entitlements = online::entitlements::minecraftapi::getEntitlementsClient()) {
		if (!entitlements->OnEntitlementsLoaded.IsBoundToObject(this)) {
			OnEntitlementsLoadedHandle = entitlements->OnEntitlementsLoaded.AddUObject(this, &UMinecraftAPIEntitlementsRepository::OnEntitlementsLoaded);
			OnEntitlementsLoadFailedHandle = entitlements->OnEntitlementsLoadFailed.AddUObject(this, &UMinecraftAPIEntitlementsRepository::OnEntitlementsLoadFailed);

			if (AuthData) {
				entitlements->Request({ AuthData->Xuid });
			} else {
				UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Missing required authenticated user data. Unable to load entitlements."));
				entitlements->OnEntitlementsLoaded.Remove(OnEntitlementsLoadedHandle);
				entitlements->OnEntitlementsLoadFailed.Remove(OnEntitlementsLoadFailedHandle);
#ifndef STEAM_BUILD
				if (PLATFORM_WINDOWS) {
					OnEntitlementsRequestFailed.Broadcast();
				}
#endif
			}
		}
	} else {		
		UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Unable to load entitlements. DungeonsAPIClient module is unavailable."));
		OnEntitlementsRequestFailed.Broadcast();
	}
	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END
