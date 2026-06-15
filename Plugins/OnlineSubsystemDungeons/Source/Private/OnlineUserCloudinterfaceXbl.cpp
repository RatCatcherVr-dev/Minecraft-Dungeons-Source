#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineJsonSerializer.h"
#include "OnlineAchievementsInterface.h"
#include "OnlineStats.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineAsyncTaskManager.h"
#include "utils.h"
#include "HrLog.h"
#include "title_storage_c.h"
#include "CommandLine.h"
#include "errors.h"
#include "OnlineUserCloudinterfaceXbl.h"

#if CLOUDSAVE_ENABLED


#define CLOUD_CHECKHRWITHOUTFAKERROR(H)		SanityCheckHR(hr,__FUNCTION__,__LINE__,false)
#define CLOUD_CHECKHRWITHFAKERROR(H)		SanityCheckHR(hr,__FUNCTION__,__LINE__,true)
#define CLOUD_FAKEERROR						{ HRESULT hr = S_OK;  if ( !CLOUD_CHECKHRWITHFAKERROR(hr)) return false; }

#define XBCLOUD_UNUSED(A)					(void)A

const auto StorageType = XblTitleStorageType::Universal;
const auto MaxFileSize = 256 * 1024;

static FOnlineSubsystemDungeons* GetOnlineSubsystemDungeons()
{
	auto OnlineSub = IOnlineSubsystem::Get();
	return static_cast<FOnlineSubsystemDungeons*>(OnlineSub);
}

static FOnlineIdentityDungeons* GetOnlineIdentityDungeons()
{
	auto Identity = GetOnlineSubsystemDungeons()->GetIdentityInterface().Get();
	return static_cast<FOnlineIdentityDungeons*>(Identity);
}

static uint64_t NetIdToXblId(const FUniqueNetId& NetId)
{
	auto User = GetOnlineIdentityDungeons()->GetUserFromNetId(NetId);
	return User->UserId;
}

static XblContextHandle NetIdToXblContextHandle(const FUniqueNetId& NetId)
{
	auto User = GetOnlineIdentityDungeons()->GetUserFromNetId(NetId);
	return User->XBLcontext;
}

static const char* GetScid()
{
	return OnlineSubsystemConstants::MULTIPLAYERSCID;
}

template<std::size_t MaxSize>
static void CopyString(const char* Source, char(&Dest)[MaxSize])
{
	auto Len = strlen(Source);
	check(Len > 0 && Len < MaxSize);
	STRCPY(Dest, Source);
}
//#pragma clang optimize off
static XblTitleStorageBlobMetadata CreateStorageMetaData(const FUniqueNetId& UserId, FStringArg FileNameIn )
{
	XblTitleStorageBlobMetadata md = { };

	md.blobType = XblTitleStorageBlobType::Binary;
	md.storageType = StorageType;
	time(&md.clientTimestamp);

	auto FileName = std::string(WSTRTOSTR(FileNameIn));
	auto DisplayPrefix = "";
	auto DisplayName = std::string(DisplayPrefix) + FileName;

	CopyString(FileName.c_str(), md.blobPath);
	CopyString(GetScid(), md.serviceConfigurationId);
	CopyString(DisplayName.c_str(), md.displayName);

	md.xboxUserId = NetIdToXblId(UserId);

	return md;
}

//#pragma clang optimize on

FOnlineUserCloudXbl::FOnlineUserCloudXbl() : FOnlineUserCloudCommonBase ("FOnlineUserCloudXbl")
{
	CLOUD_LOGFUNCTION;
}

FOnlineUserCloudXbl::~FOnlineUserCloudXbl()
{
	CLOUD_LOGFUNCTION;
}

bool FOnlineUserCloudXbl::WriteUserFile(const FUniqueNetId& UserId, FStringArg FileName, TArray<uint8>& InFileContents)
{
	auto SizeOfFile = InFileContents.Num();
	CLOUD_LOG("WriteUserFile : %s : Size = %d", WSTRTOSTR(FileName)  , SizeOfFile );

	XblTitleStorageBlobMetadata md = CreateStorageMetaData(UserId,FileName);

	bool bValid = SizeOfFile <= MaxFileSize && SizeOfFile > 0;
	check(bValid);
	if (!bValid)
		return false;

	FOnlineUserCloudFileContents FileContents = std::make_unique< TArray<uint8>>();
	*FileContents = InFileContents;

	auto Task = AsyncTasks::CreateAsyncBlock([&,md,FileName, FileContents](XAsyncBlock* AsyncBlock)
	{
		HRESULT hr = XblTitleStorageUploadBlobResult(AsyncBlock, const_cast<XblTitleStorageBlobMetadata*>(&md));
		CLOUD_CHECKHRWITHFAKERROR(hr);
		auto bSuccess = SUCCEEDED(hr);
		if (bSuccess)
		{
			auto User = GetUserInfo(UserId);
			if (!User->DoesFileExit(FileName))
				User->AddFile(FileName, FileName, FileContents->Num());
			User->SetFileContents(FileName, FileContents);
		}
		CLOUD_LOG("WriteUserFile : %s : %s", WSTRTOSTR(FileName), OkFailed(bSuccess) );
		TriggerOnWriteUserFileCompleteDelegates(bSuccess, UserId, FileName);

	}, GetOnlineSubsystemDungeons()->GetQueueHandle());
	
	CLOUD_FAKEERROR;

	auto User = GetUserInfo(UserId);
	auto hr = XblTitleStorageUploadBlobAsync(
		NetIdToXblContextHandle(UserId),
		md,
		FileContents->GetData(), FileContents->Num(),
		XblTitleStorageETagMatchCondition::NotUsed, XBL_TITLE_STORAGE_DEFAULT_UPLOAD_BLOCK_SIZE,
		Task
	);

	return CLOUD_CHECKHRWITHOUTFAKERROR(hr);
}


bool FOnlineUserCloudXbl::ReadUserFile(const FUniqueNetId& UserId, FStringArg FileName)
{
	CLOUD_LOG("ReadUserFile : %s", WSTRTOSTR(FileName));

	XblTitleStorageBlobMetadata md = CreateStorageMetaData(UserId, FileName);
	FOnlineUserCloudFileContents FileContents = FOnlineUserCloudInfo::CreateFileContents(MaxFileSize);

	auto Task = AsyncTasks::CreateAsyncBlock([&,md, FileName, FileContents](XAsyncBlock* AsyncBlock)
	{
		HRESULT hr = XblTitleStorageDownloadBlobResult(AsyncBlock, const_cast<XblTitleStorageBlobMetadata*>(&md));
		CLOUD_CHECKHRWITHFAKERROR(hr);
		auto bSuccess = SUCCEEDED(hr);
		CLOUD_LOG("ReadUserFile : %s : %s", WSTRTOSTR(FileName), OkFailed(bSuccess));
		if (bSuccess)
		{
			CLOUD_LOG("ReadUserFile : %s : Size = %d", WSTRTOSTR(FileName),md.length);
			check(md.length > 0 && md.length < MaxFileSize);
			FileContents->SetNum(md.length, true);
			GetUserInfo(UserId)->SetFileContents(FileName,FileContents);
		}
		TriggerOnReadUserFileCompleteDelegates(bSuccess, UserId, FileName);
	}, GetOnlineSubsystemDungeons()->GetQueueHandle());

	check(FileContents->Num()!=0);

	CLOUD_FAKEERROR;

	auto hr = XblTitleStorageDownloadBlobAsync(
		NetIdToXblContextHandle(UserId),
		md,
		FileContents->GetData(), FileContents->Num(),
		XblTitleStorageETagMatchCondition::NotUsed, nullptr, XBL_TITLE_STORAGE_DEFAULT_DOWNLOAD_BLOCK_SIZE,
		Task
	);
	return CLOUD_CHECKHRWITHOUTFAKERROR(hr);
}
void FOnlineUserCloudXbl::EnumerateUserFiles(const FUniqueNetId& UserId)
{
	bool r = EnumerateUserFilesPrivate(UserId);
	if (!r)
		TriggerOnEnumerateUserFilesCompleteDelegates(false, UserId);
}

bool FOnlineUserCloudXbl::EnumerateUserFilesPrivate(const FUniqueNetId& UserId)
{
	CLOUD_LOG("EnumerateUserFiles : UserId = %s" , WSTRTOSTR(UserId.ToString()) );

	GetUserInfo(UserId)->Clear();

	auto Task = AsyncTasks::CreateAsyncBlock([&](XAsyncBlock* AsyncBlock)
	{
		CLOUD_LOG("EnumerateUserFiles : Finished");

		XblTitleStorageBlobMetadataResultHandle handle = nullptr;
		HRESULT hr = XblTitleStorageGetBlobMetadataResult(AsyncBlock, &handle);
		bool bSuccess = SUCCEEDED(hr);
		if (bSuccess)
		{
			const XblTitleStorageBlobMetadata* items = nullptr;
			size_t itemsSize = 0;
			hr = XblTitleStorageBlobMetadataResultGetItems(handle, &items, &itemsSize);
			CLOUD_CHECKHRWITHFAKERROR(hr);
			bSuccess = SUCCEEDED(hr);
			if (bSuccess)
			{
				CLOUD_LOG("EnumerateUserFiles : %s" ,OkFailed(true) );
				for (size_t i = 0; i != itemsSize; i++)
				{
					auto& h = items[i];
					GetUserInfo(UserId)->AddFile(h.blobPath, h.blobPath, h.length);
					CLOUD_LOG("EnumerateUserFiles : Found item %s", h.blobPath);
				}
			}
			else
				CLOUD_LOG("EnumerateUserFiles : Failed_0");
		}
		else
		{
			if (hr == HTTP_E_STATUS_NOT_FOUND)
			{
				CLOUD_LOG("EnumerateUserFiles : HTTP_E_STATUS_NOT_FOUND ( enumaration was empty )");
				bSuccess = true;
			}
			else
			{
				CLOUD_CHECKHRWITHFAKERROR(hr);
				CLOUD_LOG("EnumerateUserFiles : Failed_1");
			}
		}
		XblTitleStorageBlobMetadataResultCloseHandle(handle);

		TriggerOnEnumerateUserFilesCompleteDelegates(bSuccess, UserId);
	}, GetOnlineSubsystemDungeons()->GetQueueHandle());

	CLOUD_FAKEERROR;

	auto ContextHandle = NetIdToXblContextHandle(UserId);
	auto xboxUserId = NetIdToXblId(UserId);

	HRESULT hr = XblTitleStorageGetBlobMetadataAsync(
		ContextHandle,
		GetScid(),
		StorageType,
		"",
		xboxUserId,
		0,
		0,
		Task
	);
	return CLOUD_CHECKHRWITHOUTFAKERROR(hr);
}


bool FOnlineUserCloudXbl::DeleteUserFile(const FUniqueNetId& UserId, FStringArg FileName, bool bShouldCloudDelete, bool bShouldLocallyDelete)
{
	CLOUD_LOG("DeleteUserFile : %s", WSTRTOSTR(FileName));

	check(bShouldCloudDelete == true);
	check(bShouldLocallyDelete == false);

	XblTitleStorageBlobMetadata md = CreateStorageMetaData(UserId, FileName);

	auto Task = AsyncTasks::CreateAsyncBlock([&,md, FileName](XAsyncBlock* AsyncBlock)
	{
		HRESULT hr = XAsyncGetStatus(AsyncBlock, false);
		CLOUD_CHECKHRWITHFAKERROR(hr);
		auto bSuccessfull = SUCCEEDED(hr);
		if (bSuccessfull)
			ClearFile(UserId, FileName);
		CLOUD_LOG("DeleteUserFile : %s : %s", WSTRTOSTR(FileName) , OkFailed(bSuccessfull) );
		TriggerOnDeleteUserFileCompleteDelegates(bSuccessfull, UserId, FileName);
	}, GetOnlineSubsystemDungeons()->GetQueueHandle());

	CLOUD_FAKEERROR;

	auto hr = XblTitleStorageDeleteBlobAsync(
		NetIdToXblContextHandle(UserId),
		md,
		false,
		Task
	);
	return CLOUD_CHECKHRWITHOUTFAKERROR(hr);
}

bool FOnlineUserCloudXbl::RequestUsageInfo(const FUniqueNetId& UserId)
{
	CLOUD_LOG("RequestUsageInfo");

	auto Task = AsyncTasks::CreateAsyncBlock([&](XAsyncBlock* AsyncBlock)
	{
		size_t usedBytes = 0;
		size_t quotaBytes = 0;
		HRESULT hr = XblTitleStorageGetQuotaResult(AsyncBlock, &usedBytes, &quotaBytes);
		CLOUD_CHECKHRWITHFAKERROR(hr);
		auto bSuccessfull = SUCCEEDED(hr);
		CLOUD_LOG("RequestUsageInfo : %s" , OkFailed(bSuccessfull));
		if (bSuccessfull)
		{
			CLOUD_LOG("usedBytes = %d"  , usedBytes);
			CLOUD_LOG("quotaBytes = %d " , quotaBytes);
		}
		TriggerOnRequestUsageInfoCompleteDelegates(bSuccessfull, UserId, usedBytes, quotaBytes);
		
	}, GetOnlineSubsystemDungeons()->GetQueueHandle());

	CLOUD_FAKEERROR;

	auto hr = XblTitleStorageGetQuotaAsync(
		NetIdToXblContextHandle(UserId),
		GetScid(),
		StorageType,
		Task
	);
	return CLOUD_CHECKHRWITHOUTFAKERROR(hr);
}

const char* FOnlineUserCloudXbl::OkFailed(bool b)
{
	return FOnlineUserCloudUtil::CloudOkFailed(b);
}

bool FOnlineUserCloudXbl::SanityCheckHR(HRESULT& hr, const char* pFunctionName, int Line, bool bFakeError)
{
	bool bSucceeded = SUCCEEDED(hr);
#if CLOUD_WANTDEBUGSTUFF
	if (bFakeError && bSucceeded )
	{
		if (FOnlineUserCloudUtil::HasSimulatedFailure() )
		{
			CLOUD_LOG("Fake cloud error");
			hr = HTTP_E_STATUS_NOT_SUPPORTED;
			bSucceeded = false;
		}
	}
#else
	XBCLOUD_UNUSED(bFakeError);
#endif

	if (!bSucceeded)
	{
		char Buffer[1024];
		SPRINTF(Buffer, "%s %d", pFunctionName, Line);
		HrLog(hr, Buffer);
	}
	return bSucceeded;
}


#endif