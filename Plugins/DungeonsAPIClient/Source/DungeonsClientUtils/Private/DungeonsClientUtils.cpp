#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsClientUtils.h"

class FDungeonsClientUtils : public IDungeonsClientUtils {

public:
	void StartupModule() override;
};

IMPLEMENT_MODULE( FDungeonsClientUtils, DungeonsClientUtils )

void FDungeonsClientUtils::StartupModule() {
}
