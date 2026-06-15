#include "ConfigUtil.h"
#include "EngineMinimal.h"

namespace dungeonsapiclient { namespace utils {
	bool isDevelopmentMode() {
		auto developmentMode = false;
		
		if (GConfig) {
			GConfig->GetBool(TEXT("Environment"), TEXT("Development"), developmentMode, GEngineIni);
		}
		
		return developmentMode;
	}
}}
