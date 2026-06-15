#pragma once
#include "SubOSS.h"

#include "OnlineUserCloudCommon.h"

namespace online {
namespace Crossplay {


class UserCloud : public SubOSS {
public:
	UserCloud(const SubsystemRepo&);
	~UserCloud();

	bool ClearFiles(const FUniqueNetId& UserId);
	bool GetFileContents(const FUniqueNetId& UserId, const FString& FileName, TArray<uint8>& FileContents);
	void EnumerateUserFiles(const FUniqueNetId& UserId);
	TArray<FCloudFileHeader> GetUserFileList(const FUniqueNetId& UserId);
	bool ReadUserFile(const FUniqueNetId& UserId, const FString& FileName);
	bool WriteUserFile(const FUniqueNetId& UserId, const FString& FileName, TArray<uint8>& FileContents);
	bool DeleteUserFile(const FUniqueNetId& UserId, const FString& FileName);

	void Login(const FUniqueNetId& UserId, TFunction<void(bool)> CompleteCallback);

	bool AreCloudFileOperationsCompleted() const;
	void ClearErrors();
	bool HadAnyErrors() const;
	bool AreCloudServicesEnabled();


private:
	bool LoginImpl(const FUniqueNetId& UserId);
	bool IsUsingPlayfabCloudServices() const;
	void OnLoginCompleted(bool bSuccessfull, const FUniqueNetId& UserId);
	static void PlayfabLoginCallBackGetAuthTokenAsync(FString token, FString issuerId, const FUniqueNetId& UserId);
	static void CompleteLogin(bool bSuccessfull, const FUniqueNetId& UserId);

	// Callbacks
	void OnDeleteUserFileComplete(bool bSuccessfull, const FUniqueNetId& UserId, const FString& File);
	void OnRequestUsageInfoComplete(bool bSuccessfull, const FUniqueNetId& UserId, int64 usedBytes, const TOptional<int64>& quotaBytes);
	void OnWriteUserFileProgress(int BytesWritten, const FUniqueNetId& UserId, const FString& FileName);
	void OnWriteUserFileComplete(bool bSuccessfull, const FUniqueNetId& UserId, const FString& FileName);
	void OnReadUserFileComplete(bool bSuccessfull, const FUniqueNetId& UserId, const FString& FileName);
	void OnEnumerateUserFilesComplete(bool bSuccessfull, const FUniqueNetId& UserId);

	FOnlineUserCloudCommonBase* GetUserCloudInterface();
	bool StartCloudOperation(bool r);
	void IncrementErrors();
	void OnCloudOperationCompleted(bool bSuccessfull);
	void DecCloudFileOperations();
	void AddCloudFileOperations();

	int32						mCloudFileOperationCount = 0;
	int32						mErrorCount = 0;
	static UserCloud*			GetInstance();
	static UserCloud*			spInstance;
	FTimerHandle				mTimeOutHandle;
	bool						mTimedOut = false;
	std::function<void(bool)>	mLoginCompleteCallback;
};
}
}
