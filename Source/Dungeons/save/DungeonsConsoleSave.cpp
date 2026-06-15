#include "Dungeons.h"
#include "DungeonsConsoleSave.h"
#include "PlatformFeatures.h"
#include "SaveGameSystem.h"
#include "SaveSpinnerInterface.h"
#include "Perforce/p4api-2015.2/include/p4/stdhdrs.h"
#include "QueuedThreadPool.h"


#if CONSOLE_SAVE_SYSTEM

class FAsyncConsoleSaveTask : public FNonAbandonableTask
{
public:
	FAsyncConsoleSaveTask(const FString &filename, int userIndex) 
		: mFilename(filename)
		, mUserIndex(userIndex)
	{
	}

	virtual bool Operation() = 0;

	virtual ~FAsyncConsoleSaveTask() {};

	static ISaveGameSystem* GetSaveSystem()
	{
		return IPlatformFeaturesModule::Get().GetSaveGameSystem();
	}

	void DoWork()
	{
		USaveSpinnerInterface::SetIsSpinnerVisible(true);
		bool bResult = Operation();
		USaveSpinnerInterface::SetIsSpinnerVisible(false);
		DungeonsConsoleSave::Instance()->DecrementAsyncTasksInflight();
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncSave, STATGROUP_ThreadPoolAsyncTasks);
	}
protected:
	FString mFilename;
	int mUserIndex;
};


class FAsyncSave : public FAsyncConsoleSaveTask
{
public:
	FAsyncSave(const FString &filename, int userIndex, const std::string &data) : FAsyncConsoleSaveTask(filename, userIndex)
	{
		mSaveBlob.SetNum(data.size());
		FMemory::Memcpy(mSaveBlob.GetData(), data.c_str(), data.size());
	}
protected:
	TArray<uint8> mSaveBlob;

	virtual bool Operation()
	{
		return  GetSaveSystem()->SaveGame(false, *mFilename, mUserIndex, mSaveBlob);
	}

};

class FAsyncLoad : public FAsyncConsoleSaveTask
{

public:
	FAsyncLoad(const FString &filename, int userIndex, TArray<uint8> &data) : FAsyncConsoleSaveTask(filename, userIndex)
		, mSaveBlob(data)
	{
	}

protected:
	TArray<uint8> mSaveBlob;

	virtual bool Operation()
	{
		bool bSuccessfull = GetSaveSystem()->LoadGame(false, *mFilename, mUserIndex, mSaveBlob);
		DungeonsConsoleSave::Instance()->BroadcastLoadCompleted(bSuccessfull, mFilename, mUserIndex, mSaveBlob);
		return bSuccessfull;
	}
};

class FAsyncDelete : public FAsyncConsoleSaveTask
{
public:
	FAsyncDelete(const FString &filename, int userIndex) : FAsyncConsoleSaveTask(filename, userIndex)
	{
	}
protected:
	virtual bool Operation()
	{
		return GetSaveSystem()->DeleteGame(false, *mFilename, mUserIndex);
	}
};

std::unique_ptr<DungeonsConsoleSave> DungeonsConsoleSave::spInstance;

DungeonsConsoleSave* DungeonsConsoleSave::Instance()
{
	check(spInstance.get());
	return spInstance.get();
}

DungeonsConsoleSave::DungeonsConsoleSave()
{
	ConsoleSaveDataThreadPool.Initialize();
	FGameDelegates::Get().GetExtendedSaveGameInfoDelegate() = FExtendedSaveGameInfoDelegate::CreateStatic(ExtendedSaveGameInfoDelegateStatic);
	mNumAsyncTasksInFlight = 0;

}

int DungeonsConsoleSave::GetNumAsyncTasksInflight() const
{
	return mNumAsyncTasksInFlight;
}


DungeonsConsoleSave::~DungeonsConsoleSave()
{
	check(mNumAsyncTasksInFlight == 0);
	ConsoleSaveDataThreadPool.Reset();
}

void DungeonsConsoleSave::Init()
{
	spInstance = std::make_unique<DungeonsConsoleSave>();

}

void DungeonsConsoleSave::Shutdown()
{
	while (spInstance)
	{
		if (spInstance->GetNumAsyncTasksInflight() != 0)
			FPlatformProcess::Sleep(0.2f);
		else
			spInstance = nullptr;
	}
}

//D11.PS - Save system wrapper functions
void DungeonsConsoleSave::SaveAsync(SaveMetaData &inMetaData, int userIndex, const std::string &data) {

	IncrementAsyncTasksInflight();
#if PLATFORM_PS4
	MetaData.Add(inMetaData.saveName, inMetaData);
#endif
	(new FAutoDeleteAsyncTask<FAsyncSave>(inMetaData.saveName, userIndex, data))->StartBackgroundTask(ConsoleSaveDataThreadPool.GetThreadPool());
}


void DungeonsConsoleSave::IntegretyCheck(int userIndex)
{
	IPlatformFeaturesModule::Get().GetSaveGameSystem()->IntegretyCheck(userIndex);
}

void DungeonsConsoleSave::LoadNonASync(const FString &filename, int userIndex, TArray<uint8> &saveData)
{
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!saveSystem->DoesSaveGameExist(*filename, userIndex))
		return;
	USaveSpinnerInterface::SetIsSpinnerVisible(true);
	bool bSuccessfull = saveSystem->LoadGame(false, *filename, userIndex, saveData);
	USaveSpinnerInterface::SetIsSpinnerVisible(false);
}

void DungeonsConsoleSave::Load(const FString &filename, int userIndex, TArray<uint8> &saveData)
{
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (saveSystem->DoesSaveGameExist(*filename, userIndex))
	{
		IncrementAsyncTasksInflight(); 
		(new FAutoDeleteAsyncTask<FAsyncLoad>(filename, userIndex, saveData))->StartBackgroundTask(ConsoleSaveDataThreadPool.GetThreadPool());// saveSystem->LoadGame(false, *filename, userIndex, saveData);
	}
	else
	{
		BroadcastLoadCompleted(false, filename, userIndex, TArray<uint8>());
	}
}

void DungeonsConsoleSave::DeleteAsync(const FString &filename, int userIndex)
{
	IncrementAsyncTasksInflight();
	(new FAutoDeleteAsyncTask<FAsyncDelete>(filename, userIndex))->StartBackgroundTask(ConsoleSaveDataThreadPool.GetThreadPool());
}

void DungeonsConsoleSave::ListSaves(int userIndex, TArray<FString> &saveList)
{
	//D11.PS this function only exists on modified engine.
#if PLATFORM_XBOXONE || PLATFORM_PS4 || PLATFORM_SWITCH
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	saveSystem->ListSaveGames(userIndex, saveList);
#endif
}

void DungeonsConsoleSave::BroadcastLoadCompleted(bool bSuccessfull, FString filename, int32 userIndex, TArray<uint8> mSaveBlob)
{
	DungeonsConsoleSave::OnAsyncLoadFinished.Broadcast(bSuccessfull, filename, userIndex, mSaveBlob);
}

void DungeonsConsoleSave::ExtendedSaveGameInfoDelegateStatic(const TCHAR* SaveName, const EGameDelegates_SaveGame Key, FString& Value)
{
	Instance()->ExtendedSaveGameInfoDelegate(SaveName, Key, Value);
}

void DungeonsConsoleSave::IncrementAsyncTasksInflight()
{
	mNumAsyncTasksInFlight++;

}
void DungeonsConsoleSave::DecrementAsyncTasksInflight()
{
	mNumAsyncTasksInFlight--;
}


void DungeonsConsoleSave::ExtendedSaveGameInfoDelegate(const TCHAR* SaveName, const EGameDelegates_SaveGame Key, FString& Value) {
	static const int32 MAX_SAVEGAME_SIZE = 8 * 1024 * 1024;
	if (SaveMetaData* metadata = MetaData.Find(SaveName)) {
		switch (Key)
		{
		case EGameDelegates_SaveGame::Icon:
			Value = metadata->iconPath;
			break;
		case EGameDelegates_SaveGame::MaxSize:
			Value = FString::Printf(TEXT("%i"), MAX_SAVEGAME_SIZE);
			break;
		case EGameDelegates_SaveGame::Title:
			Value = metadata->title;
			break;
		case EGameDelegates_SaveGame::SubTitle:
			Value = metadata->subTitle;
			break;
		case EGameDelegates_SaveGame::Detail:
			Value = metadata->details;
#if PLATFORM_PS4
			MetaData.Remove(SaveName);
#endif
			break;
		default:
			break;
		}
	}
}
#endif