#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsLiveOps.h"
#include <memory>

class FDungeonsLiveOps : public IDungeonsLiveOps {

public:
	void StartupModule() override;
	std::shared_ptr<LiveOpsClient> LiveOps() override;

private:
	std::shared_ptr<LiveOpsClient> Client = nullptr;
};

IMPLEMENT_MODULE(FDungeonsLiveOps, DungeonsLiveOps)

void FDungeonsLiveOps::StartupModule() {
	Client = std::make_shared<LiveOpsClient>();
}

std::shared_ptr<LiveOpsClient> FDungeonsLiveOps::LiveOps() {
	return Client;
}
