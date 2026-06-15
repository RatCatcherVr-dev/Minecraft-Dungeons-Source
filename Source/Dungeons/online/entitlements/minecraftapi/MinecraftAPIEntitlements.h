#pragma once

#include "CoreMinimal.h"
#include "EntitlementsClient.h"

namespace online { namespace entitlements { namespace minecraftapi {
	std::shared_ptr<EntitlementsClient> getEntitlementsClient();
}}}
