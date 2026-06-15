#include "PlayfabCloud.h"
#include "OnlineUserCloudCommon.h"
#include "httpinternal.h"

#if CLOUDSAVE_ENABLED


#if CLOUD_WANTDEBUGSTUFF
#define CLOUD_LOGFAILEDCALLBACK(A)	LogLambdaFailed(A);
#define CLOUD_LOGSUCCESSCALLBACK(A) LogLambdaSucceed(A);
#else
#define CLOUD_LOGFAILEDCALLBACK(A) 
#define CLOUD_LOGSUCCESSCALLBACK(A) 
#endif

class FOnlineUserCloudPlayfab : public FOnlineUserCloudCommonBase
{
public:
	FOnlineUserCloudPlayfab() : FOnlineUserCloudCommonBase("FOnlineUserCloudPlayfab")
	{
		CLOUD_LOGFUNCTION;
	}

#if CLOUD_WANTDEBUGSTUFF
	void LogLambdaSucceed(FStringArg String)
	{
		CLOUD_LOG("Callback : %s Succeed", WSTRTOSTR(String));
	}
	void LogLambdaFailed(FStringArg String)
	{
		CLOUD_LOG("Callback : %s Failed", WSTRTOSTR(String));
	}
#endif
	virtual bool ReadUserFile(FUniqueNetIdArg UserId, FStringArg  FileName)
	{
		CLOUD_LOG("ReadUserFile : %s", WSTRTOSTR(FileName));
		auto failedCallback = [&, FileName](const FString& content,EHttpResponseCodes::Type Error)
		{
			CLOUD_LOGFAILEDCALLBACK("ReadUserFile");
			if (Error == EHttpResponseCodes::NotFound)
			{
				CLOUD_LOG("ReadUserFile : FAILED %s : EHttpResponseCodes::NotFound ( amazon response )", WSTRTOSTR(FileName));
				TriggerOnReadUserFileCompleteDelegates(true, UserId, FileName);
			}
			else
				TriggerOnReadUserFileCompleteDelegates(false, UserId, FileName);
		};

		auto successCallback = [&, FileName](const TArray<uint8>& contents)
		{
			CLOUD_LOGSUCCESSCALLBACK("ReadUserFile");
			GetUserInfo(UserId)->SetFileContents(FileName, contents);
			TriggerOnReadUserFileCompleteDelegates(true, UserId, FileName);
		};

		auto response = http_internal::createResponseLambda({ {} }, successCallback, failedCallback);

		auto Url = GetUserInfo(UserId)->GetFileInfo(FileName).DLName;
		return  http_internal::doHttpRequestBinaryDownload(Url, response);
	}


	void LogElapsedSeconds(int Step )
	{
		auto Time = mStopWatch.GetElapsedSeconds();
		CLOUD_LOG("WriteUserFileStep(%d) took : %f", Step , Time);
	}

	virtual bool WriteUserFile(FUniqueNetIdArg UserId, FStringArg FileName, TArray<uint8>& FileContents)
	{
		CLOUD_LOG("WriteUserFile : %s", WSTRTOSTR(FileName));
		mStopWatch.SetToNow();
		auto failedCallback = [&,FileName,FileContents](FString content)
		{
			CLOUD_LOGFAILEDCALLBACK("WriteUserFile");
			LogElapsedSeconds(-1);

			CLOUD_LOG("AbortFileUpload failed : %s", WSTRTOSTR(FileName));
			WriteUserFile0(UserId, FileName, FileContents);
		};

		auto successCallback = [&, FileName, FileContents](FJsonObject& object)
		{
			CLOUD_LOGSUCCESSCALLBACK("WriteUserFile");
			LogElapsedSeconds(-1);
			CLOUD_LOG("AbortFileUpload Success : %s", WSTRTOSTR(FileName));
			WriteUserFile0(UserId, FileName, FileContents);
		};

		return DoRestAPIRequest<DataModel::CloudAbortFileUploadsRequest>(FileName, failedCallback, successCallback);
	}


	virtual bool DeleteUserFile(FUniqueNetIdArg UserId, FStringArg  FileName, bool bShouldCloudDelete, bool bShouldLocallyDelete)
	{
		CLOUD_LOG("DeleteUserFile : %s", WSTRTOSTR(FileName));
		auto failedCallback = [&,FileName](FString content)
		{
			CLOUD_LOGFAILEDCALLBACK("DeleteUserFile");
			TriggerOnDeleteUserFileCompleteDelegates(false, UserId, FileName);
		};

		auto successCallback = [&, FileName](FJsonObject& object)
		{
			CLOUD_LOGSUCCESSCALLBACK("DeleteUserFile");
			ClearFile(UserId, FileName);
			TriggerOnDeleteUserFileCompleteDelegates(true, UserId, FileName);
		};

		return DoRestAPIRequest<DataModel::CloudDeleteFilesRequest>(FileName, failedCallback, successCallback);
	}

	virtual void EnumerateUserFiles(FUniqueNetIdArg UserId)
	{
		CLOUD_LOGFUNCTION;

		GetUserInfo(UserId)->Clear();

		auto failedCallback = [&](FString content)
		{
			CLOUD_LOGFAILEDCALLBACK("EnumerateUserFiles");
			EnumerateUserFilesFailed(UserId);
		};

		auto successCallback = [&](FJsonObject& object)
		{
			CLOUD_LOGSUCCESSCALLBACK("EnumerateUserFiles");
			AddEnumartedFiles(UserId, object);
			TriggerOnEnumerateUserFilesCompleteDelegates(true, UserId);
		};

		auto bSuccess = DoRestAPIRequest<DataModel::CloudGetFilesRequest>("", failedCallback, successCallback);
		if (!bSuccess)
			EnumerateUserFilesFailed(UserId);
	}

private:
	template<class C, class A, class B>
	bool DoRestAPIRequest(FStringArg FileName, A failedCallback, B successCallback)
	{
		auto pPlayer = GetPlayfabPlayer();
		std::vector<FString> FileList = { FileName };
		auto Request = C(
			GetTitleId(),
			pPlayer->GetEntityToken(),
			pPlayer->GetPlayFabId(),
			FileList
		);
		auto response = http_internal::createResponseLambda({ {} }, successCallback, failedCallback);
		return  http_internal::doHttpRequest(Request, response);
	}

	void WriteUserFile0(FUniqueNetIdArg UserId, FStringArg FileName, const TArray<uint8>& FileContents)
	{
		CLOUD_LOGFUNCTION;
		auto failedCallback = [&,FileName](FString content)
		{
			CLOUD_LOGFAILEDCALLBACK("WriteUserFile0");
			LogElapsedSeconds(0);
			WriteUserFileFailed(UserId, FileName);
		};

		auto successCallback = [&, FileName,FileContents](FJsonObject& object)
		{
			CLOUD_LOGSUCCESSCALLBACK("WriteUserFile0");
			LogElapsedSeconds(0);
			WriteUserFile1(object, UserId, FileContents);
		};


		bool bSuccess = DoRestAPIRequest<DataModel::CloudInitiateFileUploadsRequest>(FileName, failedCallback, successCallback);
		if (!bSuccess)
			WriteUserFileFailed(UserId, FileName);
	}


	void WriteUserFileFailed(FUniqueNetIdArg UserId, FStringArg FileName)
	{
		TriggerOnWriteUserFileCompleteDelegates(false, UserId, FileName);
	}
	void EnumerateUserFilesFailed(FUniqueNetIdArg UserId)
	{
		TriggerOnEnumerateUserFilesCompleteDelegates(false, UserId);
	}

	void WriteUserFile2(FUniqueNetIdArg UserId, FStringArg  FileName,  const TArray<uint8>& FileContents)
	{
		CLOUD_LOGFUNCTION;
		auto failedCallback = [&, FileName](FString content)
		{
			CLOUD_LOGFAILEDCALLBACK("WriteUserFile2");
			LogElapsedSeconds(2);
			WriteUserFileFailed(UserId, FileName);
		};

		auto successCallback = [&, FileName, FileContents](FJsonObject& object)
		{
			CLOUD_LOGSUCCESSCALLBACK("WriteUserFile2");
			LogElapsedSeconds(2);
			auto User = GetUserInfo(UserId);
			if (!User->DoesFileExit(FileName))
				User->AddFile(FileName, FileName, FileContents.Num());
			User->SetFileContents(FileName, FileContents);
			TriggerOnWriteUserFileCompleteDelegates(true, UserId, FileName);
		};
		auto bSuccess = DoRestAPIRequest<DataModel::CloudFinalizeFileUploadsRequest>(FileName, failedCallback, successCallback);
		if (!bSuccess)
			WriteUserFileFailed(UserId, FileName);
	};

	void WriteUserFile1(FJsonObject& object, FUniqueNetIdArg UserId, const TArray<uint8>& Contents)
	{
		CLOUD_LOGFUNCTION;
		auto UploadDetailsArray = object.GetObjectField("data")->GetArrayField("UploadDetails");

		for (auto it : UploadDetailsArray)
		{
			auto FileName = it->AsObject()->GetStringField("FileName");
			auto UploadUrl = it->AsObject()->GetStringField("UploadUrl");
			auto failedCallback = [&, FileName](FString content)
			{
				CLOUD_LOGFAILEDCALLBACK("WriteUserFile1");
				LogElapsedSeconds(1);
				WriteUserFileFailed(UserId, FileName);
			};

			auto successCallback = [&, FileName, Contents]()
			{
				CLOUD_LOGSUCCESSCALLBACK("WriteUserFile1");
				LogElapsedSeconds(1);
				WriteUserFile2(UserId, FileName, Contents);
			};
			auto response = http_internal::createResponseLambda({ {} }, successCallback, failedCallback);
			bool bSuccess = http_internal::doHttpRequestBinaryUpload(UploadUrl, response, Contents);
			if (!bSuccess)
			{
				WriteUserFileFailed(UserId, FileName);
				break;
			}
		}
	};

	void AddEnumartedFiles(FUniqueNetIdArg UserId, FJsonObject& object)
	{
		auto MetaDataArray = object.GetObjectField("data")->GetObjectField("Metadata");
		auto Values = MetaDataArray->Values;
		for (auto it : Values)
		{
			auto FleInfo = it.Value->AsObject();
			auto FileName = FleInfo->GetStringField("FileName");
			auto DownloadUrl = FleInfo->GetStringField("DownloadUrl");
			auto Size = FleInfo->GetIntegerField("Size");
			GetUserInfo(UserId)->AddFile(FileName, DownloadUrl, Size);
			CLOUD_LOG("AddEnumartedFiles[%d] : FileName = %s / DownloadUrl = %s /  Size = %d ", GetUserInfo(UserId)->GetNumFiles(),  WSTRTOSTR(FileName) , WSTRTOSTR(DownloadUrl),Size );
		}
		CLOUD_LOG("Enumarated %d files", GetUserInfo(UserId)->GetNumFiles());
	}

	PlayFabPlayer* GetPlayfabPlayer()
	{
		return PlayfabServices::GetPlayfabPlayer();
	}

	FString GetTitleId()
	{
		return PlayfabServices::GetTitleId();
	}


	FOnlineStopWatch mStopWatch;
};

IOnlineUserCloud* PlayfabServices::GetUserCloudInterface()
{
	check(IsInGameThread());
	static FOnlineUserCloudPlayfab* spInstance = new FOnlineUserCloudPlayfab();

	return spInstance;
}
#else

IOnlineUserCloud* PlayfabServices::GetUserCloudInterface()
{
	return nullptr;
}

#endif