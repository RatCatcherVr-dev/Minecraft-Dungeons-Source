#pragma once
#include "CommonTypes.h"
#include "GameDelegates.h"

//D11.PS - Wrapper class for the console saves

#define CONSOLE_SAVE_SYSTEM (PLATFORM_PS4 || PLATFORM_SWITCH || PLATFORM_XBOXONE)

#if CONSOLE_SAVE_SYSTEM

namespace ConsoleSave {
	const FString SaveFileAppend = FString(TEXT("Character"));
}

struct FConsoleSaveDataThreadPool
{

	~FConsoleSaveDataThreadPool()
	{
		Reset();
	}

	void Initialize()
	{
		// initialize worker thread pools
		if (FPlatformProcess::SupportsMultithreading())
		{
			int32 NumThreadsInThreadPool = 1;

			Pool = FQueuedThreadPool::Allocate();
			verify(Pool->Create(NumThreadsInThreadPool, 128 * 1024, TPri_Normal));
		}
	}

	void Reset()
	{
		if (Pool != nullptr)
		{
			Pool->Destroy();
			Pool = nullptr;
		}
	}

	FQueuedThreadPool* GetThreadPool()
	{
		return Pool;
	}

private:
	FQueuedThreadPool* Pool = nullptr;
};

struct SaveMetaData {
	FString saveName;
	FString title;
	FString subTitle;
	FString details;
	FString iconPath;
};

DECLARE_MULTICAST_DELEGATE_FourParams(FOnAsyncLoadFinished, bool, FString, int32, TArray<uint8>&);
class DungeonsConsoleSave
{
public:
	DungeonsConsoleSave();
	~DungeonsConsoleSave();

	static DungeonsConsoleSave* Instance();
	static void Init();
	static void Shutdown();

	void SaveAsync(SaveMetaData &inMetaData, int userIndex, const std::string &data);
	void DeleteAsync(const FString &filename, int userIndex);
	//static bool LoadAsync(const FString &filename, int userIndex, TArray<uint8> &saveData);

	void Load(const FString &filename, int userIndex, TArray<uint8> &saveData);
	void LoadNonASync(const FString &filename, int userIndex, TArray<uint8> &saveData);
	void ListSaves(int userIndex, TArray<FString> &saveList);

	void BroadcastLoadCompleted(bool bSuccessfull, FString filename, int32 userIndex, TArray<uint8> mSaveBlob);

	FOnAsyncLoadFinished OnAsyncLoadFinished;
	FConsoleSaveDataThreadPool ConsoleSaveDataThreadPool;
	void IntegretyCheck(int userIndex);
	int GetNumAsyncTasksInflight() const;
	void IncrementAsyncTasksInflight();
	void DecrementAsyncTasksInflight();
protected:
	// D11.SSN
	static void ExtendedSaveGameInfoDelegateStatic(const TCHAR* SaveName, const EGameDelegates_SaveGame Key, FString& Value);
	 void ExtendedSaveGameInfoDelegate(const TCHAR* SaveName, const EGameDelegates_SaveGame Key, FString& Value);
	TMap<FString, SaveMetaData> MetaData;
	std::atomic<int> mNumAsyncTasksInFlight;
	static std::unique_ptr<DungeonsConsoleSave> spInstance;
};

#endif
