#include "Dungeons.h"
#include "EntitlementsRepositoryFactory.h"
#include "online/entitlements/CompositeEntitlementsRepository.h"

#include "minecraftapi/MinecraftAPIEntitlementsRepository.h"

#if PLATFORM_WINDOWS
#include "WindowsStore/WindowsStoreEntitlementsRepository.h"
#ifdef EPIC_STORE_BUILD
#include "EpicStore/EpicStoreEntitlementsRepository.h"
#endif
#ifdef STEAM_BUILD
#include "Steam/SteamEntitlementsRepository.h"
#endif
#elif PLATFORM_XBOXONE
#include "XboxOne/XboxOneEntitlementsRepository.h"
#elif PLATFORM_SWITCH
#include "Switch/SwitchEntitlementsRepository.h"
#elif PLATFORM_PS4
#include "PS4/PS4EntitlementsRepository.h"
#endif
#include "util/ConfigFileUtil.h"
#include <Anticheat.hpp>


namespace online { namespace entitlements {

	ANTICHEAT_NO_OPTIMIZATION_BEGIN

void addEntitlementRepository(UCompositeEntitlementsRepository* compositeRepository, const EEntitlementsSource entitlementsSource, UEntitlementsRepository* entitlementRepository) {
	ANTICHEAT_VIRT_BEGIN

	entitlementRepository->Init(entitlementsSource);
	compositeRepository->RegisterRepository(entitlementsSource, entitlementRepository);

	ANTICHEAT_VIRT_END
}

UEntitlementsRepository* createRepository() {
	ANTICHEAT_VIRT_BEGIN
	
	auto* compositeRepository = NewObject<UCompositeEntitlementsRepository>();
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	addEntitlementRepository(compositeRepository, EEntitlementsSource::WindowsStore, NewObject<UWindowsStoreEntitlementsRepository>());
#else
	addEntitlementRepository(compositeRepository, EEntitlementsSource::MinecraftNet, NewObject<UMinecraftAPIEntitlementsRepository>());
#if defined(EPIC_STORE_BUILD)
	addEntitlementRepository(compositeRepository, EEntitlementsSource::EpicStore, NewObject<UEpicStoreEntitlementsRepository>());
#elif defined(STEAM_BUILD)
	addEntitlementRepository(compositeRepository, EEntitlementsSource::Steam, NewObject<USteamEntitlementsRepository>());
#elif PLATFORM_XBOXONE
	addEntitlementRepository(compositeRepository, EEntitlementsSource::XBL, NewObject<UXboxOneEntitlementsRepository>());
#elif PLATFORM_SWITCH
	addEntitlementRepository(compositeRepository, EEntitlementsSource::Nintendo, NewObject<USwitchEntitlementsRepository>());
#elif PLATFORM_PS4
	addEntitlementRepository(compositeRepository, EEntitlementsSource::PSN, NewObject<UPS4EntitlementsRepository>());
#endif
#endif
	
	return compositeRepository;
	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END

}}
