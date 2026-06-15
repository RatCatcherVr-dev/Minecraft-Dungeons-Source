#pragma once

#include "UnrealString.h"
#include <Anticheat.hpp>

namespace minecraft {
	namespace epicstore {
		static const FString EpicGameServicesTokenId = ANTICHEAT_STRINGS("EpicGameServicesToken");
#if UE_BUILD_SHIPPING
		static const FString EpicGameServicesTokenUrl = ANTICHEAT_STRINGS("https://api.epicgames.dev");
#else
		static const FString EpicGameServicesTokenUrl = ANTICHEAT_STRINGS("https://api-staging.epicgames.dev");
#endif
	}
}
