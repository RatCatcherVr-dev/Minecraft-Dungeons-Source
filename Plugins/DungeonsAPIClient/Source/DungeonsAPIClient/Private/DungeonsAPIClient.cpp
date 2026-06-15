#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsAPIClient.h"
#include "IDungeonsTrials.h"
#include "IDungeonsAuth.h"
#include "IDungeonsEntitlements.h"
#include "IDungeonsLiveOps.h"
#include "IDungeonsClient.h"

class FDungeonsAPIClient : public IDungeonsAPIClient {

public:
	std::shared_ptr<TrialsClientManager> Trials() override;
	std::shared_ptr<DungeonsAuthenticator> Auth() override;
	std::shared_ptr<EntitlementsClient> Entitlements() override;
	std::shared_ptr<LiveOpsClient> LiveOps() override;

	void Init(const DungeonsClientInfo&, FTimerManager&) override;
};

IMPLEMENT_MODULE( FDungeonsAPIClient, DungeonsAPIClient )

std::shared_ptr<DungeonsAuthenticator> FDungeonsAPIClient::Auth() {
	check(IDungeonsAuth::IsAvailable() && "Auth module is unavailable.");
	return IDungeonsAuth::Get().Auth();
}

std::shared_ptr<TrialsClientManager> FDungeonsAPIClient::Trials() {
	check(IDungeonsTrials::IsAvailable() && "Trials module is unavailable.");
	return IDungeonsTrials::Get().Trials();
}

std::shared_ptr<EntitlementsClient> FDungeonsAPIClient::Entitlements() {
	check(IDungeonsEntitlements::IsAvailable() && "Entitlements module is unavailable.");
	return IDungeonsEntitlements::Get().Entitlements();
}

std::shared_ptr<LiveOpsClient> FDungeonsAPIClient::LiveOps() {
	check(IDungeonsLiveOps::IsAvailable() && "Seasons module is unavailable.");
	return IDungeonsLiveOps::Get().LiveOps();
}

void FDungeonsAPIClient::Init(const DungeonsClientInfo& clientInfo, FTimerManager& timerManager) {
	check(IDungeonsClient::IsAvailable() && "Client module is unavailable.");
	IDungeonsClient::Get().Init(clientInfo, timerManager);
}
