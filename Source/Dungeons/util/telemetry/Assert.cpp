#include "Dungeons.h"
#include "Assert.h"
#include "Analytics.h"
#include "Engine.h"
#include "DungeonsGameInstance.h"

namespace dungeons
{
	void fensure_networked(bool condition, UObject* worldContextObject
		, const char* filename
		, int line
		, const char* functionName
		, AssertSeverityLevel severity
		, const char* message)
	{
		if (!condition) {
			return;
		}

		std::stringstream file;
		file << filename << "#" << line << ": " << functionName << std::endl;

		analytics::Analytics::GetInstance().FireEventAssert(UTF8_TO_TCHAR(file.str().c_str()), message ? UTF8_TO_TCHAR(message) : FString());
	}
}
