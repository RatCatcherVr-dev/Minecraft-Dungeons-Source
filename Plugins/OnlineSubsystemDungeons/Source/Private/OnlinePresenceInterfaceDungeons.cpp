#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlinePresenceInterfaceDungeons.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineIdentityDungeons.h"
#include "xsapiServicesInclude.h"
#include "utils.h"

FOnlinePresenceDungeons::FOnlinePresenceDungeons(FOnlineSubsystemDungeons* InDungeonsSubsystem)	: DungeonsSubsystem(InDungeonsSubsystem) {
}

void FOnlinePresenceDungeons::SetPresence(const FUniqueNetId& User, const FOnlineUserPresenceStatus& Status, const FOnPresenceTaskCompleteDelegate& Delegate)
{
	// Potentially working implementation have existed, check source control.
	// Removed because it had issues (translations for Camp for example), and wasn't worth maintaining if it's not used.

	Delegate.ExecuteIfBound(User, false);
}

void FOnlinePresenceDungeons::QueryPresence(const FUniqueNetId& User, const FOnPresenceTaskCompleteDelegate& Delegate)
{
	FOnlineIdentityDungeons* IdentityInterface = DungeonsSubsystem->GetDungeonsIdentityInterface();
	if (!IdentityInterface)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Presence] Unable to SetPresence, IdentityInterface is NULL"));
		return;
	}
	auto XBLcontext = IdentityInterface->GetXBLContextHandle(0);
	auto Block = std::make_unique<XAsyncBlock>();
	auto TaskCompleteBlock = std::make_unique< PresenceTaskCompleteBlock>(User, Delegate);
	Block->callback = [](XAsyncBlock* BlockPtr)
	{
		std::unique_ptr<XAsyncBlock> Block{ BlockPtr };
		std::unique_ptr<PresenceTaskCompleteBlock> TaskCompleteBlock{ (PresenceTaskCompleteBlock*)Block->context };

		XblPresenceRecordHandle handle;
		XblPresenceGetPresenceResult(Block.get(), &handle);
		if (handle == nullptr)
		{
			UE_LOG_ONLINE(Log, TEXT("[Presence] Failed to get presence records"));
			TaskCompleteBlock->mDelegate.ExecuteIfBound(TaskCompleteBlock->mUser, false);
		}
		else
		{
			const XblPresenceDeviceRecord* DeviceRecords;
			size_t DeviceRecordCount;

			XblPresenceRecordGetDeviceRecords(handle, &DeviceRecords, &DeviceRecordCount);
			for (auto DeviceRecordIndex = 0; DeviceRecordIndex < DeviceRecordCount; DeviceRecordIndex++)
			{
				for (auto DeviceTitleRecordIndex = 0;
					DeviceTitleRecordIndex < DeviceRecords[DeviceRecordIndex].titleRecordsCount;
					DeviceTitleRecordIndex++)
				{
					auto title = &DeviceRecords[DeviceRecordIndex].titleRecords[DeviceTitleRecordIndex];

					UE_LOG_ONLINE(Log, TEXT("[Presence] TitleName='%s', TitleId='%u', Presence='%s'")
						, ANSI_TO_TCHAR(title->titleName)
						, title->titleId
						, ANSI_TO_TCHAR(title->richPresenceString)
					);
				}
			}
			TaskCompleteBlock->mDelegate.ExecuteIfBound(TaskCompleteBlock->mUser, true);
		}
	};
	Block->context = TaskCompleteBlock.release();
	Block->queue = nullptr;

	XblPresenceGetPresenceAsync(XBLcontext, IdentityInterface->GetUserFromNetId(User)->UserId, Block.release());
}

EOnlineCachedResult::Type FOnlinePresenceDungeons::GetCachedPresence(const FUniqueNetId& User, TSharedPtr<FOnlineUserPresence>& OutPresence)
{
	TSharedRef<FOnlineUserPresenceDungeons>* foundUser = CachedPresence.Find(FUniqueNetIdDungeons(User));
	if(!foundUser)
	{
		return EOnlineCachedResult::NotFound;
	}

	OutPresence = MakeShared<FOnlineUserPresenceDungeons>(foundUser->Get());
	return EOnlineCachedResult::Success;
}

EOnlineCachedResult::Type FOnlinePresenceDungeons::GetCachedPresenceForApp(const FUniqueNetId& LocalUserId, const FUniqueNetId& User, const FString& AppId, TSharedPtr<FOnlineUserPresence>& OutPresence)
{
	//TODO : Not implemented yet
	return EOnlineCachedResult::NotFound;
}