#include "DemoUtils.h"
#include "util/ConfigFileUtil.h"
#include "DungeonsGameInstance.h"
#include <ConsoleManager.h>
#include <CoreGlobals.h>

bool UDemoUtils::IsDemo()
{
	return configfile::IsDemo();
}

bool UDemoUtils::TryStartDemo(const UObject* WorldContextObject)
{
	auto* gi = Cast<UDungeonsGameInstance>(WorldContextObject->GetWorld()->GetGameInstance());
	if (!gi || !GConfig)
		return false;

	FString demoSetupValue;
	GConfig->GetString(TEXT("Demo"), TEXT("Setup"), demoSetupValue, GGameIni);
	if (!demoSetupValue.IsEmpty()) {
		FOutputDevice& out = *GLog;
		FConsoleManager::Get().ProcessUserConsoleInput(*("Dungeons.Command.RunIni " + demoSetupValue), out, WorldContextObject->GetWorld());
		return true;
	}
	return false;
}

// D11.SSN
bool UDemoUtils::SkipSplash() {
	return configfile::SkipSplash();
}

