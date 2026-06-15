#include "SteamEntitlementsRepository.h"
#include <Anticheat.hpp>



void USteamEntitlementsRepository::ConfigureForPlatform()
{
	ensure(GetSource() == EEntitlementsSource::Steam);
	mDlcInstalledCallback.SetSteamEntitelmentRepo(this);

}

ANTICHEAT_NO_OPTIMIZATION_BEGIN

void USteamEntitlementsRepository::RequestEntitlements()
{
	ANTICHEAT_VIRT_BEGIN
	RemoveAllEntitlements();
	AddEntitlementsForPlatform("game_dungeons", GetSource());

#ifdef STEAM_BUILD
	int32 DLCCount = SteamApps()->GetDLCCount();
	for (int i = 0; i < DLCCount; ++i) 
	{
		AppId_t appId;
		char name[128];
		bool available;
		if (SteamApps()->BGetDLCDataByIndex(i, &appId, &available, name, 128))
		{
			if (SteamApps()->BIsDlcInstalled(appId))
			{
				TArray< FStringFormatArg > args;
				args.Add(FStringFormatArg(static_cast<int>(appId)));
				FString PackageName = FString::Format(TEXT("{0}"), args);
				AddEntitlementsForPlatform(PackageName, GetSource());
			}
		}
	}
#endif
	ANTICHEAT_VIRT_END

	OnEntitlementsProvided.Broadcast(GetEntitlements());
}

#ifdef STEAM_BUILD
void DlcInstalledCallback::OnDlcInstalled(DlcInstalled_t* pCallback)
{
	mEntitlementRepo->RequestEntitlements();
}
#endif



ANTICHEAT_NO_OPTIMIZATION_END

