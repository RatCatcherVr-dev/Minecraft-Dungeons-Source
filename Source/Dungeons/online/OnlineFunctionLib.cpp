#include "Dungeons.h"
#include "OnlineFunctionLib.h"

extern TAutoConsoleVariable<int32> CVarAllowFriendsSidebar;

bool UOnlineFunctionLib::AllowToggleFriendsSidebar()
{
	return CVarAllowFriendsSidebar.GetValueOnGameThread() > 0? true: false;

}
