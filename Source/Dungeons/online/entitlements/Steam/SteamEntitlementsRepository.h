#pragma once



#include "UObject/ObjectMacros.h"
#include "../EntitlementsRepository.h"

#ifdef STEAM_BUILD
#pragma push_macro("ARRAY_COUNT")
#undef ARRAY_COUNT

THIRD_PARTY_INCLUDES_START

#include "steam/steam_api.h"

THIRD_PARTY_INCLUDES_END

#pragma pop_macro("ARRAY_COUNT")

#endif

#include "SteamEntitlementsRepository.generated.h"

class USteamEntitlementsRepository;

class DlcInstalledCallback
{
public:

	void SetSteamEntitelmentRepo(USteamEntitlementsRepository *EntitlementRepo)
	{
		mEntitlementRepo = EntitlementRepo;
	}

#ifdef STEAM_BUILD
	STEAM_CALLBACK(DlcInstalledCallback, OnDlcInstalled, DlcInstalled_t, OnDlcInstalledCallback);

	DlcInstalledCallback() 
		: OnDlcInstalledCallback(this, &DlcInstalledCallback::OnDlcInstalled)
		, mEntitlementRepo(nullptr)
	{
	}
#endif

private:
	USteamEntitlementsRepository *mEntitlementRepo;
};



UCLASS()
class DUNGEONS_API USteamEntitlementsRepository : public UEntitlementsRepository {

	GENERATED_BODY()
public:	
	void RequestEntitlements() override;
protected:
	void ConfigureForPlatform() override;

	DlcInstalledCallback mDlcInstalledCallback;
};



#ifdef STEAM_BUILD
#pragma warning(default: 4265) // Here you disable the compiler throwing warning with code C4265.
#endif
