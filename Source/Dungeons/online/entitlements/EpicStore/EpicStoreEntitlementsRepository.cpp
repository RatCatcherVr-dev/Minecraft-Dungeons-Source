#include "EpicStoreEntitlementsRepository.h"
#include <Anticheat.hpp>
#ifdef EPIC_STORE_BUILD
#include "EpicServices.h"
#endif

void UEpicStoreEntitlementsRepository::ConfigureForPlatform()
{
	ensure(GetSource() == EEntitlementsSource::EpicStore);
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN

void UEpicStoreEntitlementsRepository::RequestEntitlements() 
{
	ANTICHEAT_VIRT_BEGIN
	RemoveAllEntitlements();
	//AddEntitlementsForPlatform("game_dungeons", GetSource()); // the main game entitlement should also be provided by the Epic Store
#ifdef EPIC_STORE_BUILD
	using namespace minecraft::epicstore;

	static FEpicServicesModule &EosModule = FEpicServicesModule::Get();
	EosModule.RequestEntitlements(
		FOnEpicEntitlementsReceived::CreateUObject(this, &UEpicStoreEntitlementsRepository::OnEntitlementsReceived),
		FOnEpicEntitlementsRequestFailed::CreateUObject(this, &UEpicStoreEntitlementsRepository::OnEntitlementsReqFailed)
	);
#endif
	ANTICHEAT_VIRT_END
}

void UEpicStoreEntitlementsRepository::OnEntitlementsReceived(TArray<FString> entitlements)
{
	ANTICHEAT_VIRT_BEGIN
	// TODO: validate the NewEntitlements, there is a validator interface ready to be implemented

	for(const FString &e: entitlements)
		AddEntitlementsForPlatform(e, GetSource());

	OnEntitlementsProvided.Broadcast(GetEntitlements());
	ANTICHEAT_VIRT_END
}

void UEpicStoreEntitlementsRepository::OnEntitlementsReqFailed()
{
	ANTICHEAT_VIRT_BEGIN
	OnEntitlementsRequestFailed.Broadcast();
	ANTICHEAT_VIRT_END
}

ANTICHEAT_NO_OPTIMIZATION_END
