#pragma once

#include "CoreMinimal.h"
#include "OnlineUserCloudCommon.h"

#if CLOUDSAVE_ENABLED

class FOnlineUserCloudXbl : public FOnlineUserCloudCommonBase
{
public:
	FOnlineUserCloudXbl();
	virtual ~FOnlineUserCloudXbl();

	virtual void EnumerateUserFiles(FUniqueNetIdArg);
	virtual bool ReadUserFile(FUniqueNetIdArg, FStringArg FileName);
	virtual bool WriteUserFile(FUniqueNetIdArg, FStringArg FileName, TArray<uint8>& FileContents) ;
	virtual bool DeleteUserFile(FUniqueNetIdArg, FStringArg FileName, bool bShouldCloudDelete, bool bShouldLocallyDelete);
	virtual bool RequestUsageInfo(FUniqueNetIdArg);
private:
	bool EnumerateUserFilesPrivate(FUniqueNetIdArg);
	static const char* OkFailed(bool b);
	bool SanityCheckHR(HRESULT& hr, const char* pFunctionName, int Line,bool bFakeError);
};

#endif

