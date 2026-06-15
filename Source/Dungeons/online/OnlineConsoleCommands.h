#pragma once

class UWorld;
class APlayerController;
DECLARE_LOG_CATEGORY_EXTERN(LogOnlineConsole, Display, All);
namespace DungeonsDebugOnline {

	void markNetRelevantActors(const UWorld* world, const APlayerController* playerController);
};