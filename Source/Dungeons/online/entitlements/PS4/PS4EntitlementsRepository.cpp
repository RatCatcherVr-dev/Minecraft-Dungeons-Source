#include "Dungeons.h"
#include "PS4EntitlementsRepository.h"
#include "PS4/PS4Application.h"
#include <app_content.h>

void UPS4EntitlementsRepository::ConfigureForPlatform()
{
	ensure(GetSource() == EEntitlementsSource::PSN);
	FPS4Application::GetPS4Application()->OnAdditionalContentInstalledDelegate.AddUObject(this, &UPS4EntitlementsRepository::RequestEntitlements);
}

void UPS4EntitlementsRepository::RequestEntitlements() {
	RemoveAllEntitlements();
	//D11.PS - Add the base game entitlement
	AddEntitlementsForPlatform("game_dungeons", GetSource());

	SceNpServiceLabel ServiceLabel = 0;
	uint32_t ListNum = 0;
	uint32_t HitNum;

	int Ret = sceAppContentGetAddcontInfoList(ServiceLabel, nullptr, ListNum, &HitNum);

	if (Ret == SCE_OK && HitNum > 0)
	{
		/* Prepare the required buffers */
		std::vector<SceAppContentAddcontInfo> List;
		List.resize(HitNum);
		ListNum = HitNum;

		/* Get a list of additional content information for which the entitlement is valid */
		Ret = sceAppContentGetAddcontInfoList(ServiceLabel, &List[0], ListNum, &HitNum);

		if (Ret == SCE_OK && HitNum > 0) 
		{
			for (int i = 0; i < HitNum; i++)
			{
				FString PackageName = List[i].entitlementLabel.data;
				AddEntitlementsForPlatform(PackageName, GetSource());
			}
		}
	}

	OnEntitlementsProvided.Broadcast(GetEntitlements());
}
