#include "Dungeons.h"
#include "CompositeEntitlementsRepository.h"
#include <Anticheat.hpp>
#include "online/sessions/OnlineUtil.h"

extern TAutoConsoleVariable<int32> CVarEnableEntitlements;

#define IS_CONSOLE (PLATFORM_XBOXONE || PLATFORM_PS4 || PLATFORM_SWITCH)

FRegisteredSource::FRegisteredSource()
	: Repository(nullptr)
	, State(ESourceState::Pending) {	
}

FRegisteredSource::FRegisteredSource(UEntitlementsRepository* repository, const ESourceState state)
	: Repository(repository)
	, State(state) {	
}

void UCompositeEntitlementsRepository::RegisterRepository(const EEntitlementsSource source, UEntitlementsRepository* repository) {
	ANTICHEAT_VIRT_BEGIN
	if (!RegisteredSources.Find(source)) {
		RegisteredSources.Add(source, FRegisteredSource(repository, ESourceState::Pending));
		BindDelegates(source);	
	}	
	ANTICHEAT_VIRT_END
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
void UCompositeEntitlementsRepository::RequestEntitlements() {

	ANTICHEAT_VIRT_BEGIN

#if !UE_BUILD_SHIPPING
		if (CVarEnableEntitlements.GetValueOnGameThread() >= 1) {
			RemoveEntitlements(RETLAMBDA(it.GetEntitlementsSource() == EEntitlementsSource::Development));
#endif		
			for (auto& registeredSource : RegisteredSources) {
				RequestEntitlements(registeredSource.Key, registeredSource.Value);
			}
#if !UE_BUILD_SHIPPING
		}
		else {
			//Not using entitlement checking, we just own all of them!
			SetEntitlements({
				FEntitlement("game_dungeons", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_dlc_1", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_dlc_2", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_dlc_3", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_dlc_4", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_dlc_5", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_dlc_6", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_hero_cape_1", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_hero_skin_1", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_hero_skin_2", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_hero_pet_1", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_season_cape_1", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_season_pet_1", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_season_skin_1", EEntitlementsSource::Development),
				FEntitlement("game_dungeons_season_skin_2", EEntitlementsSource::Development)
				});
			if (online::IsUsingOnlineFeatures()) {
				for (auto& registeredSource : RegisteredSources) {
					RequestEntitlements(registeredSource.Key, registeredSource.Value);
				}
			} else {
				OnEntitlementsProvided.Broadcast(GetEntitlements());
			}
		}
#endif

	ANTICHEAT_VIRT_END
}

bool UCompositeEntitlementsRepository::IsEntitlementTamperedWith(const FEntitlement& entitlement, const FString& expectedEntitlementName, const FString& expectedSigningUser) {
	ANTICHEAT_VIRT_BEGIN

	if (const auto* registeredSource = RegisteredSources.Find(entitlement.GetEntitlementsSource())) {
		return registeredSource->Repository->IsEntitlementTamperedWith(entitlement, expectedEntitlementName, expectedSigningUser);
	}
	return Super::IsEntitlementTamperedWith(entitlement, expectedEntitlementName, expectedSigningUser);

	ANTICHEAT_VIRT_END
}

void UCompositeEntitlementsRepository::RequestEntitlements(const EEntitlementsSource source, FRegisteredSource& registeredSource) {	
	ANTICHEAT_VIRT_BEGIN	

	if (registeredSource.State != ESourceState::RequestSent) {
		registeredSource.State = ESourceState::RequestSent;
		registeredSource.Repository->RequestEntitlements();	
	}

	ANTICHEAT_VIRT_END
}

void UCompositeEntitlementsRepository::OnEntitlementsProvidedByRegisteredRepository(const TArray<FEntitlement>& entitlements, const EEntitlementsSource source) {
	ANTICHEAT_VIRT_BEGIN	

	auto registeredSource = RegisteredSources.Find(source);
	registeredSource->State = ESourceState::Succeeded;

	RemoveEntitlements(RETLAMBDA(it.GetEntitlementsSource() == source));
	AddEntitlements(entitlements);
	/* 
	Console entitlement response is triggered before we are signed in to MinecraftServices. 
	To be able to give the user entitlement feedback (early and potentially with no internet connection)
	we handle consoles differently.
	*/
	if (IS_CONSOLE && source == online::entitlements::getPrimaryEntitlementsSource()) {
		HandleConsoleEntitlement(source);
	}
	else {
		BroadcastIfAllSourcesReceived();
	}

	ANTICHEAT_VIRT_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UCompositeEntitlementsRepository::OnRegisteredRepositoryEntitlementsRequestFailed(const EEntitlementsSource source) {	

	auto registeredSource = RegisteredSources.Find(source);
	registeredSource->State = ESourceState::Failed;

	BroadcastIfAllSourcesReceived();
}

void UCompositeEntitlementsRepository::BindDelegates(const EEntitlementsSource source) {
	auto registeredSource = RegisteredSources.Find(source);
	if (!registeredSource->Repository->OnEntitlementsProvided.IsBoundToObject(this)) {
		const auto successCallback = &UCompositeEntitlementsRepository::OnEntitlementsProvidedByRegisteredRepository;
		const auto successHandle = registeredSource->Repository->OnEntitlementsProvided.AddUObject(this, successCallback, source);
		registeredSource->OnEntitlementProvidedHandle = successHandle;

		const auto failCallback = &UCompositeEntitlementsRepository::OnRegisteredRepositoryEntitlementsRequestFailed;
		const auto failHandle = registeredSource->Repository->OnEntitlementsRequestFailed.AddUObject(this, failCallback, source);
		registeredSource->OnEntitlementsRequestFailedHandle = failHandle;
	}
}

void UCompositeEntitlementsRepository::HandleConsoleEntitlement(const EEntitlementsSource source) {
	auto registeredSource = RegisteredSources.Find(source);
	if (registeredSource->State == ESourceState::Succeeded) {
		OnEntitlementsProvided.Broadcast(GetEntitlements());
	}
	else {
		OnEntitlementsRequestFailed.Broadcast();
	}
}

void UCompositeEntitlementsRepository::BroadcastIfAllSourcesReceived() {
	auto received = 0;
	auto failed = false;
	
	for (const auto& source : RegisteredSources) {
		if (source.Value.State == ESourceState::Succeeded) {
			received++;
		} else if (source.Value.State == ESourceState::Failed) {
			received++;
			failed = true;
		}
	}

	if (received == RegisteredSources.Num()) {
		ResetSourceStates();

		if (failed) {
			UE_LOG(LogDungeonsEntitlements, Warning, TEXT("A registered entitlement repository reported failure when requesting entitlements."));
			OnEntitlementsRequestFailed.Broadcast();
		} else {
			OnEntitlementsProvided.Broadcast(GetEntitlements());
		}
	}
}

void UCompositeEntitlementsRepository::ResetSourceStates() {
	for (auto& source : RegisteredSources) {
		source.Value.State = ESourceState::Pending;
	}
}

