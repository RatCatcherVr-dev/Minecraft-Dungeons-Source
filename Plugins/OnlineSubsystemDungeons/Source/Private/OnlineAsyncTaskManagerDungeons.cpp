#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineAsyncTaskManagerDungeons.h"


void FOnlineAsyncTaskManagerDungeons::OnlineTick()
{
	check(DungeonsSubsystem);
	check(FPlatformTLS::GetCurrentThreadId() == OnlineThreadId || !FPlatformProcess::SupportsMultithreading());
}