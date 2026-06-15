#include "Dungeons.h"
#include "MinecraftAPIEntitlements.h"
#include "IDungeonsAPIClient.h"

namespace online { namespace entitlements { namespace minecraftapi {
	std::shared_ptr<EntitlementsClient> getEntitlementsClient() {
		if (IDungeonsAPIClient::IsAvailable()) {
			return IDungeonsAPIClient::Get().Entitlements();
		}
		return nullptr;
	}
}}}
