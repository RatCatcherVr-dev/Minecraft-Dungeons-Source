#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsAuth.h"
#include <memory>
#include "DungeonsAuthenticator.h"

class FDungeonsAuth : public IDungeonsAuth {

public:
	void StartupModule() override;

	std::shared_ptr<DungeonsAuthenticator> Auth() override;

private:
	std::shared_ptr<DungeonsAuthenticator> APIAuthenticator = nullptr;
};

IMPLEMENT_MODULE( FDungeonsAuth, DungeonsAuth )

void FDungeonsAuth::StartupModule() {
	APIAuthenticator = std::make_shared<DungeonsAuthenticator>();
}

std::shared_ptr<DungeonsAuthenticator> FDungeonsAuth::Auth() {
	return APIAuthenticator;
}
