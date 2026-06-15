#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsTrials.h"
#include "AnonymousTrialsClient.h"

class FDungeonsTrials : public IDungeonsTrials {

public:
	void StartupModule() override;

	std::shared_ptr<TrialsClientManager> Trials() override;
private:
	std::shared_ptr<TrialsClientManager> ClientManager = nullptr;
};

IMPLEMENT_MODULE( FDungeonsTrials, DungeonsTrials )

void FDungeonsTrials::StartupModule() {
	ClientManager = make_shared<TrialsClientManager>();
}

std::shared_ptr<TrialsClientManager> FDungeonsTrials::Trials() {
	return ClientManager;
}