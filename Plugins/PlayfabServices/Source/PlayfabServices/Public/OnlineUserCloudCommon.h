#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <array>
#include <unordered_set>
#include <future>
#include "OnlineUserCloudInterface.h"
#include <OnlineSubsystem.h>


// D11.Fred : Master cloudsave enable/disable.  Wideish coverage.
#define CLOUDSAVE_ENABLED				1
#define CLOUDSAVE_PLASTERFIX_453486		CLOUDSAVE_ENABLED && 1

#if CLOUDSAVE_ENABLED


#define FStringArg const FString&
#define StringArg const std::string&
#define FUniqueNetIdArg const FUniqueNetId&

#define CLOUD_WANTDEBUGSTUFF			!UE_BUILD_SHIPPING
#define WSTRTOSTR(S)					TCHAR_TO_UTF8(*S)

#if !PLATFORM_SWITCH
#define SPRINTF sprintf_s
#define STRCPY strcpy_s
#define VSPRINTF vsprintf_s
#else
#define VSPRINTF vsprintf
#define SPRINTF sprintf
#define STRCPY strcpy
#endif

#if CLOUD_WANTDEBUGSTUFF
#define CLOUD_NOT_IMPLEMENTED_YET		PLATFORM_BREAK()
#define CLOUD_LOGFUNCTION				mLogger.Log(__FUNCTION__)
#define CLOUD_LOG(...)					mLogger.Log(__VA_ARGS__)
#else
#define CLOUD_NOT_IMPLEMENTED_YET		
#define CLOUD_LOGFUNCTION				
#define CLOUD_LOG(...)					
#endif



class FOnlineLogger
{
public:
	FOnlineLogger(const std::string& Title);
	void Log(const char* format, ...);
protected:
	int	mLogCount = 0;
	std::string mTitle;
};


class FOnlineStopWatch
{
public:
	FOnlineStopWatch() 
	{
		SetToNow();
	}

	float GetElapsedSeconds()
	{
		auto TimeNow = Now();
		auto DeltaTime = TimeNow - mTime;
		auto DeltaTimeSeconds = std::chrono::duration<float>(DeltaTime).count();
		mTime = TimeNow;
		return DeltaTimeSeconds;
	}
	void SetToNow()
	{
		mTime = Now();
	}

protected:
	std::chrono::time_point<std::chrono::system_clock> Now()
	{
		return std::chrono::system_clock::now();
	}
	std::chrono::time_point<std::chrono::system_clock>	mTime;

};

class FOnlineUserCloudUtil
{
public:
	static const char* CloudOkFailed(bool b);
	static bool HasSimulatedFailure();
	static bool HasCommandLine(const char* pCommand);
protected:
	static float GetSimulatedFailureRate();
};

inline FOnlineLogger::FOnlineLogger(const std::string& Title) : mTitle(Title)
{
}

inline void FOnlineLogger::Log(const char* format, ...)
{
	char Buffer[1024];
	va_list v;
	va_start(v, format);
	VSPRINTF(Buffer, format, v);

	UE_LOG_ONLINE(Log, TEXT("**** [%s][%d] :  %s"), *FString(mTitle.c_str()), mLogCount, *FString(Buffer));
	mLogCount++;
	va_end(v);
}


inline const char* FOnlineUserCloudUtil::CloudOkFailed(bool b)
{
	return b ? "OK" : "Failed";
}

#if CLOUD_WANTDEBUGSTUFF

inline bool FOnlineUserCloudUtil::HasCommandLine(const char* pCommand)
{
	return FParse::Param(FCommandLine::Get(), (const TCHAR*)*FString(pCommand));
}

inline bool FOnlineUserCloudUtil::HasSimulatedFailure()
{
	auto FailureChance = GetSimulatedFailureRate();
	check(FailureChance >= 0.0f && FailureChance <= 100.0f);
	return FMath::FRandRange(0.0f, 100.0f) < FailureChance;
}

inline float FOnlineUserCloudUtil::GetSimulatedFailureRate()
{
	static float sFailureChance = 0.0f;;
	static bool b = FParse::Value(FCommandLine::Get(), TEXT("CloudFailureChance"), sFailureChance);
	return sFailureChance;
}

#else	// CLOUD_WANTDEBUGSTUFF

inline bool FOnlineUserCloudUtil::HasSimulatedFailure()
{
	return false;
}

inline bool FOnlineUserCloudUtil::HasCommandLine(const char* pCommand)
{
	return false;
}


#endif	// CLOUD_WANTDEBUGSTUFF

typedef std::shared_ptr<TArray<uint8>> FOnlineUserCloudFileContents;

#define CloudHeadersArray		TArray<FCloudFileHeader>

class FOnlineUserCloudInfo
{
public:
	void AddFile(FStringArg FileName, FStringArg DlName, int Size)
	{
		check(!DoesFileExit(FileName));
		Files[FileName].first = FCloudFileHeader(FileName, DlName, Size);
	}


	void SetFileContents(FStringArg FileName, const TArray<uint8>& FileContents)
	{
		SetFileContents(FileName, std::make_shared< TArray<uint8>>(FileContents));
	}

	void SetFileContents(FStringArg FileName, FOnlineUserCloudFileContents FileContents)
	{
		check(DoesFileExit(FileName));
		Files[FileName].second = FileContents;
	}

	bool DoesFileExit(FStringArg Name)  const
	{
		return Files.find(Name) != Files.end();
	}

	bool HasFileDownloaded(FStringArg Name)  const
	{
		if (!DoesFileExit(Name))
			return false;
		return GetFileContents(Name) != nullptr;
	}

	FOnlineUserCloudFileContents GetFileContents(FStringArg Name) const
	{
		check(DoesFileExit(Name));
		return Files.at(Name).second;
	}
	FCloudFileHeader GetFileInfo(FStringArg Name) const
	{
		check(DoesFileExit(Name));
		return Files.at(Name).first;
	}

	void Clear()
	{
		Files.clear();
	}

	void DeleteFile(FStringArg Name)
	{
		Files.erase(Name);
	}

	CloudHeadersArray GetHeaders() const
	{
		CloudHeadersArray r;
		for (auto& it : Files)
		{
			auto& Value = it.second;
			r.Add(Value.first);
		}
		return r;
	}

	int GetNumFiles() const
	{
		return int(Files.size());
	}

	static FOnlineUserCloudFileContents CreateFileContents(int Size = 0)
	{
		FOnlineUserCloudFileContents FileContents = std::make_shared< TArray<uint8>>();
		if ( Size != 0 )
			FileContents->SetNum(Size);
		return FileContents;
	}
protected:
	std::map<FString, std::pair<FCloudFileHeader,FOnlineUserCloudFileContents> > Files;
};


#if CLOUDSAVE_ENABLED


class FOnlineUserCloudCommonBase : public IOnlineUserCloud
{
public:
	FOnlineUserCloudCommonBase( const char* pTitlelogText) : mLogger( pTitlelogText )
	{
	}

	virtual void CancelWriteUserFile(FUniqueNetIdArg, FStringArg)
	{
		CLOUD_NOT_IMPLEMENTED_YET;
	}
	virtual bool RequestUsageInfo(FUniqueNetIdArg)
	{
		CLOUD_NOT_IMPLEMENTED_YET;
		return false;
	}
	virtual void DumpCloudState(FUniqueNetIdArg)
	{
		CLOUD_NOT_IMPLEMENTED_YET;
	}
	virtual void DumpCloudFileState(FUniqueNetIdArg, FStringArg  FileName)
	{
		CLOUD_NOT_IMPLEMENTED_YET;
	}

	virtual bool GetFileContents(const FUniqueNetId& UserId, const FString&  FileName, TArray<uint8>& FileContents)
	{
		auto User = GetUserInfo(UserId);
		bool r = User->DoesFileExit(FileName);
		if (r)
		{
			r = User->HasFileDownloaded(FileName);
			if (r)
				FileContents = *User->GetFileContents(FileName);
		}
		else
		{
			check(User->HasFileDownloaded(FileName) == false);
		}

		return r;
	}
	virtual bool ClearFiles(const FUniqueNetId& UserId)
	{
		CLOUD_LOG("ClearFiles");
		if (UserId.IsValid())
			GetUserInfo(UserId)->Clear();
		return true;
	}
	virtual void GetUserFileList(const FUniqueNetId& UserId, CloudHeadersArray& UserFiles)
	{
		UserFiles = GetUserInfo(UserId)->GetHeaders();
	}

	virtual bool ClearFile(const FUniqueNetId& UserId, FStringArg FileName)
	{
		CLOUD_LOG("ClearFile : %s", WSTRTOSTR(FileName));
		auto User = GetUserInfo(UserId);
		bool r = User->DoesFileExit(FileName);
		if (r)
			User->DeleteFile(FileName);
		return r;
	}
protected:


	FOnlineUserCloudInfo* GetUserInfo(FUniqueNetIdArg UserId)
	{
		CLOUD_LOG("GetUserInfo");
		auto s = UserId.ToString();
		if (CloudUserInfo.find(s) == CloudUserInfo.end())
			CloudUserInfo[s] = std::make_unique<FOnlineUserCloudInfo>();
		return CloudUserInfo[s].get();
	}


	FOnlineLogger mLogger;
	std::map<FString, std::unique_ptr<FOnlineUserCloudInfo> > CloudUserInfo;
};

#endif

#endif