#include "Dungeons.h"
#include "DungeonsGameInstance.h"

namespace online { namespace entitlements {

	static FDelegateHandle OnProvidedHandle;
	
}}

static void PrintEntitlements(UWorld* world, const TArray<FEntitlement>& entitlements) {	
	if (const auto playerController = world->GetFirstPlayerController()) {		

		if (entitlements.Num() > 0) {
			playerController->ClientMessage("Entitlements...");
			for (const auto& entitlement : entitlements) {
				playerController->ClientMessage(entitlement.GetName());
			}	
		} else {
			playerController->ClientMessage("No entitlements");
		}
	}
}

static void Load(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const auto* gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	auto* entitlementsRepository = gameInstance->GetEntitlementsRepository();

	const auto& callback = [=](const TArray<FEntitlement>& entitlements) {
		PrintEntitlements(world, entitlements);
		if (online::entitlements::OnProvidedHandle.IsValid()) {
			entitlementsRepository->OnEntitlementsProvided.Remove(online::entitlements::OnProvidedHandle);
		}		
	};

	online::entitlements::OnProvidedHandle = entitlementsRepository->OnEntitlementsProvided.AddLambda(callback);

	entitlementsRepository->RequestEntitlements();
}

static void List(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const auto* gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	auto* entitlementsRepository = gameInstance->GetEntitlementsRepository();
	PrintEntitlements(world, entitlementsRepository->GetEntitlements());
}

static void Load1YearCape(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const auto* gameInstance = world->GetGameInstance<UDungeonsGameInstance>();
	auto* entitlementsRepository = gameInstance->GetEntitlementsRepository();
	entitlementsRepository->AddEntitlement("game_dungeons_one_year_anniversary_cape", EEntitlementsSource::Development);
}

static const FAutoConsoleCommand LoadEntitlements(TEXT("Dungeons.Entitlements.Load")
	, TEXT("Load entitlements from the Minecraft API")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&Load)
	, ECVF_Cheat);

static const FAutoConsoleCommand ListEntitlements(TEXT("Dungeons.Entitlements.List")
	, TEXT("List currently cached entitlements")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&List)
	, ECVF_Cheat);

static const FAutoConsoleCommand Load1YearCapeEntitlement(TEXT("Dungeons.Entitlements.Load1Year")
	, TEXT("Load 1yearanniversary cape")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&Load1YearCape)
	, ECVF_Cheat);

TAutoConsoleVariable<int32> CVarEnableEntitlements(
	TEXT("Dungeons.Entitlements.Enable"),
#if WITH_EDITOR
	0,
#else
	1,
#endif
	TEXT("Turn the entitlements system on and off.\n")
	TEXT("<=0: Don't use entitlements system at all. All players have all entitlements and no validation is done.\n")
	TEXT(">=1: Use the entitlements system.\n"),
	ECVF_Cheat);
