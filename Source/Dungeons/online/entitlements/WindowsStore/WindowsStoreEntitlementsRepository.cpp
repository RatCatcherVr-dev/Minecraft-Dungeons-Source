#include "Dungeons.h"
#include "WindowsStoreEntitlementsRepository.h"
#include "IDungeonsAPIClient.h"
#include "online/entitlements/minecraftapi/MinecraftAPIEntitlements.h"

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UWindowsStoreEntitlementsRepository::RequestEntitlements() {
	ANTICHEAT_VIRT_BEGIN
	if (const auto entitlements = online::entitlements::minecraftapi::getEntitlementsClient()) {
		if (!entitlements->OnEntitlementsLoaded.IsBoundToObject(this)) {
			OnEntitlementsLoadedHandle = entitlements->OnEntitlementsLoaded.AddUObject(this, &UWindowsStoreEntitlementsRepository::OnEntitlementsLoaded);
			OnEntitlementsLoadFailedHandle = entitlements->OnEntitlementsLoadFailed.AddUObject(this, &UWindowsStoreEntitlementsRepository::OnEntitlementsLoadFailed);

			if (AuthData) {
				if (MinecraftAuthData && MinecraftAuthData->RequestId.IsSet() && MinecraftAuthData->ProductIdList.IsSet()) {
					entitlements->Request({ AuthData->Xuid }, MinecraftAuthData->RequestId.GetValue(), MinecraftAuthData->ProductIdList.GetValue());
				}
			} else {
				UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Missing required authenticated user data. Unable to load entitlements."));
				entitlements->OnEntitlementsLoaded.Remove(OnEntitlementsLoadedHandle);
				entitlements->OnEntitlementsLoadFailed.Remove(OnEntitlementsLoadFailedHandle);
				OnEntitlementsRequestFailed.Broadcast();
			}
		}
	} else {		
		UE_LOG(LogDungeonsEntitlements, Warning, TEXT("Unable to load entitlements. DungeonsAPIClient module is unavailable."));
		OnEntitlementsRequestFailed.Broadcast();
	}
	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END

