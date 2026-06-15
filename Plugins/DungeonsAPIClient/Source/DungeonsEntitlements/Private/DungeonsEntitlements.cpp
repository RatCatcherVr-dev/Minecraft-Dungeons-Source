#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsEntitlements.h"

class FDungeonsEntitlements : public IDungeonsEntitlements {

public:
	void StartupModule() override;

	std::shared_ptr<EntitlementsClient> Entitlements() override;
private:
	std::shared_ptr<EntitlementsClient> Client = nullptr;
};

IMPLEMENT_MODULE( FDungeonsEntitlements, DungeonsEntitlements )

void FDungeonsEntitlements::StartupModule() {
	Client = make_shared<EntitlementsClient>();
}

std::shared_ptr<EntitlementsClient> FDungeonsEntitlements::Entitlements() {
	return Client;
}
