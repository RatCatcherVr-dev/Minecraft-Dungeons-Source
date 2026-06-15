#include "DungeonsPlatformUtils.h"
#include "GameFramework/InputSettings.h"

bool UDungeonsPlatformUtils::GetPlatformUsesRightForAccept() {
#if PLATFORM_PS4 || PLATFORM_SWITCH
	UInputSettings* settings = UInputSettings::GetInputSettings();
	bool swap = settings->GetPlatformUsesRightForAccept();
	return swap;
#endif
	return false;
}

bool UDungeonsPlatformUtils::GetPlatformSupportsTitleNewsNavigation() {
#if PLATFORM_WINDOWS
	return true;
#else
	return false;
#endif
}
