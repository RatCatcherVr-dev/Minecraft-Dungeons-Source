#include "UserCloud.h"
#include <PlayfabServices.h>
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"


#define ADDCLOUDCALLBACK(C)				{ CloudInterface->C##Delegates.AddRaw(this, &UserCloud::C);  }

namespace online {
namespace Crossplay {

UserCloud*  UserCloud::spInstance = nullptr;

UserCloud*	UserCloud::GetInstance()
{
	return spInstance;
}

UserCloud::UserCloud(const SubsystemRepo& subsystems) : SubOSS(subsystems) {

	check(IsInGameThread());
	check(spInstance == nullptr);
	spInstance = this;
	auto CloudInterface = GetUserCloudInterface();

	if (CloudInterface)
	{
		ADDCLOUDCALLBACK(OnRequestUsageInfoComplete);
		ADDCLOUDCALLBACK(OnDeleteUserFileComplete);
		ADDCLOUDCALLBACK(OnEnumerateUserFilesComplete);
		ADDCLOUDCALLBACK(OnWriteUserFileProgress);
		ADDCLOUDCALLBACK(OnWriteUserFileComplete);
		ADDCLOUDCALLBACK(OnReadUserFileComplete);
	}
}

UserCloud::~UserCloud() {
	check(IsInGameThread());
	check(spInstance != nullptr);
	spInstance = nullptr;
}

bool UserCloud::ClearFiles(const FUniqueNetId& UserId)
{
	return GetUserCloudInterface()->ClearFiles(UserId);
}

bool UserCloud::GetFileContents(const FUniqueNetId& UserId, const FString& FileName, TArray<uint8>& FileContents)
{
	return GetUserCloudInterface()->GetFileContents(UserId, FileName, FileContents);
}

void UserCloud::EnumerateUserFiles(const FUniqueNetId& UserId)
{

	AddCloudFileOperations();
	GetUserCloudInterface()->EnumerateUserFiles(UserId);
}

TArray<FCloudFileHeader> UserCloud::GetUserFileList(const FUniqueNetId& UserId)
{
	TArray<FCloudFileHeader> UserFiles;
	GetUserCloudInterface()->GetUserFileList(UserId, UserFiles);
	return UserFiles;
}

bool UserCloud::ReadUserFile(const FUniqueNetId& UserId, const FString& FileName)
{
	return StartCloudOperation(GetUserCloudInterface()->ReadUserFile(UserId, FileName));
}

bool UserCloud::WriteUserFile(const FUniqueNetId& UserId, const FString& FileName, TArray<uint8>& FileContents)
{
	return StartCloudOperation(GetUserCloudInterface()->WriteUserFile(UserId, FileName, FileContents));
}

bool UserCloud::DeleteUserFile(const FUniqueNetId& UserId, const FString& FileName)
{
	return StartCloudOperation(GetUserCloudInterface()->DeleteUserFile(UserId, FileName, true, false));
}

FOnlineUserCloudCommonBase* UserCloud::GetUserCloudInterface()
{
	if (IsUsingPlayfabCloudServices())
		return (FOnlineUserCloudCommonBase*)PlayfabServices::GetUserCloudInterface();
	else
	{
		if (auto* dungeonsSubsystem = Subsystems.Get(SubsystemType::Dungeons))
			return (FOnlineUserCloudCommonBase*)dungeonsSubsystem->GetUserCloudInterface().Get();
		return nullptr;
	}
}

void UserCloud::IncrementErrors()
{
	mErrorCount++;
	check(mErrorCount >= 0);
}

void UserCloud::OnCloudOperationCompleted(bool bSuccessfull)
{
	DecCloudFileOperations();
	if (!bSuccessfull)
		IncrementErrors();
}


void UserCloud::DecCloudFileOperations()
{
	mCloudFileOperationCount--;
	check(mCloudFileOperationCount >= 0);
}


void UserCloud::AddCloudFileOperations()
{
	mCloudFileOperationCount++;
	check(mCloudFileOperationCount > 0);
}


bool	UserCloud::AreCloudFileOperationsCompleted() const
{
	check(mCloudFileOperationCount >= 0);
	return mCloudFileOperationCount == 0;
}

void UserCloud::ClearErrors()
{
	mErrorCount = 0;
}

bool UserCloud::IsUsingPlayfabCloudServices() const
{
#if UE_EDITOR
	return false;
#else
	return true;
#endif
}

bool UserCloud::AreCloudServicesEnabled()
{
	return GetUserCloudInterface() != nullptr;
}

bool UserCloud::StartCloudOperation(bool r)
{
	if (r)
		AddCloudFileOperations();
	else
		IncrementErrors();
	return r;
}

bool UserCloud::HadAnyErrors() const
{
	check(mErrorCount >= 0);
	return mErrorCount > 0;
}


void UserCloud::OnDeleteUserFileComplete(bool bSuccessfull, const FUniqueNetId& UserId, const FString& File)
{
	OnCloudOperationCompleted(bSuccessfull);
}

void UserCloud::OnRequestUsageInfoComplete(bool bSuccessfull, const FUniqueNetId& UserId, int64 usedBytes, const TOptional<int64>& quotaBytes)
{
	OnCloudOperationCompleted(bSuccessfull);
}

void UserCloud::OnWriteUserFileProgress(int BytesWritten, const FUniqueNetId& UserId, const FString& FileName)
{
}

void UserCloud::OnWriteUserFileComplete(bool bSuccessfull, const FUniqueNetId& UserId, const FString& FileName)
{
	OnCloudOperationCompleted(bSuccessfull);
}

void UserCloud::OnReadUserFileComplete(bool bSuccessfull, const FUniqueNetId& UserId, const FString& FileName)
{
	OnCloudOperationCompleted(bSuccessfull);
}

void UserCloud::OnEnumerateUserFilesComplete(bool bSuccessfull, const FUniqueNetId& UserId)
{
	OnCloudOperationCompleted(bSuccessfull);
}

void UserCloud::OnLoginCompleted(bool bSuccessfull, const FUniqueNetId& UserId)
{
	OnCloudOperationCompleted(bSuccessfull);
	mLoginCompleteCallback(bSuccessfull);
}

void UserCloud::Login(const FUniqueNetId& UserId, TFunction<void(bool)> CompleteCallback)
{
	if (!IsUsingPlayfabCloudServices())
	{
		CompleteCallback(false);
		return;
	}
	mLoginCompleteCallback = CompleteCallback;

	AddCloudFileOperations();
	if (!LoginImpl(UserId))
		OnLoginCompleted(false, UserId);
}

void UserCloud::CompleteLogin(bool bSuccessfull, const FUniqueNetId& UserId)
{
	check(IsInGameThread());
	if (GetInstance() != nullptr)
		GetInstance()->OnLoginCompleted(bSuccessfull, UserId);
}

void UserCloud::PlayfabLoginCallBackGetAuthTokenAsync(FString token, FString issuerId, const FUniqueNetId& UserId)
{
	check(IsInGameThread());
	if (!GetInstance())
		return;
	auto CallBack = [&](bool bSuccess)
	{
		CompleteLogin(bSuccess, UserId);
	};

	if (!PlayfabServices::RegisterOnAuthenticationCallback({ token, issuerId, CallBack }))
		CompleteLogin(false, UserId);
}

bool UserCloud::LoginImpl(const FUniqueNetId& UserId)
{
	if (auto world = GetWorldForOnline(GetFirstActiveSubsystem()->GetInstanceName()))
	{
		if (const auto DGameInstance = world->GetGameInstance<UDungeonsGameInstance>())
		{
			if (DGameInstance->CheckConnectionStatus())
			{
				mTimedOut = false;
				world->GetTimerManager().SetTimer(mTimeOutHandle, [&] {
					mTimedOut = true;
					CompleteLogin(false, UserId);
				}, 100.0f, false);


				auto CallBack = [&](FString token, FString issuerId)
				{
					if (!mTimedOut)
					{
						if (auto world = GetWorldForOnline(GetFirstActiveSubsystem()->GetInstanceName()))
						{
							world->GetTimerManager().ClearTimer(mTimeOutHandle);
						}
						PlayfabLoginCallBackGetAuthTokenAsync(token, issuerId, UserId);
					}
				};
				return online::getIdentityInterface()->GetAuthTokenAsync("playfab_key", CallBack);
			}
		}
	}
	return false;
}

}
}
