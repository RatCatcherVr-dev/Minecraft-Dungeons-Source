#pragma once

#include "CoreMinimal.h"
#include <ObjectMacros.h>
#include "OnlineUserCloudCommon.h"
#include "CommonTypes.h"
#include "OnlineSubsystem.h"
#include "util/ZLibUtil.h"

#if CLOUDSAVE_ENABLED

class IOnlineIdentity;
class IOnlineUserCloud;
class APlayerController;
class UGlobalStateData;
class UDungeonsLocalPlayer;
class ABaseMenuPlayerController;
class UDungeonsCloudSaveBPProxy;
class APlayerCharacterSaveSlot;

#if CLOUD_WANTDEBUGSTUFF
#define USERCLOUD_LOGFUNCTION		FDungeonsCloudLoadSave::GetLogger()->Log(__FUNCTION__)
#define USERCLOUD_LOG(...)			FDungeonsCloudLoadSave::GetLogger()->Log(__VA_ARGS__)
#define USERCLOUD_LOGUSER(...)		FDungeonsCloudLoadSave::GetLogger()->Log(__VA_ARGS__)
#define USERCLOUD_LOGUSERFUNCTION	FDungeonsCloudLoadSave::GetLogger()->Log(__FUNCTION__)
#else
#define USERCLOUD_LOGFUNCTION		
#define USERCLOUD_LOG(...)			
#define USERCLOUD_LOGUSER(...)		
#define USERCLOUD_LOGUSERFUNCTION	
#endif

class FCloudArchive;
class FCloudFileUtil;

typedef int32 SystemId;
typedef TArray<uint8> SerializedData;
typedef std::shared_ptr<SerializedData> SerializedDataPtr;
typedef const uint8* SerializedDataInput;
typedef uint8* SerializedDataOutput;
typedef  TArray<APlayerCharacterSaveSlot*> SaveSlotList;
typedef  const SaveSlotList& SaveSlotListArg;

class FSaveSlotListWrapper
{
public:
	FSaveSlotListWrapper();
	~FSaveSlotListWrapper();
	void Set(SaveSlotListArg List);
	SaveSlotListArg Get();
	void Free();
private:
	SaveSlotList  mList;
};


class FDungeonsCloudLoadSaveContext
{
public:

	enum class ECloudOperation
	{
		Unknown,
		GetCharacters,
		SetCharacters
	};

	enum class ELoadSaveState
	{
		Idle,

		WaitingForLogin,
		StartEnumaratingCloudFilesDelay,
		StartEnumaratingCloudFiles,
		WaitingForCloudFilesToBeEnumarated,
		WaitingForCloudFilesToBeRead,
		DecompressNextFile,
		WaitingForCloudArchiveToBeDecompressed,
		WaitingForLocalArchiveToBeLoaded,

		WaitingForCloudArchiveToBeCompressed,
		WaitingForCloudArchiveToBeUploaded,
		WaitingForOldCloudArchivesToBeDeleted,

		UserAccountNotLinked,

		Error
	};

	FDungeonsCloudLoadSaveContext(SystemId Id);
	~FDungeonsCloudLoadSaveContext();

	void Sync();
	void Tick();

	UDungeonsLocalPlayer* GetLocalPlayer();
	bool HaveAllOperationsCompleted();
	void SetDungeonsCloudCharacters(SaveSlotListArg);
	void DownloadDungeonsCloudCharacter(APlayerCharacterSaveSlot* pSlot);
	bool IsIdle() const;
	bool IsErrored() const;
	void SanityCheck();
	bool AreCloudFileOperationsCompleted() const;
	bool AreLocalFileOperationsCompleted() const;
protected:
	UGlobalStateData* GetGlobalSaveData();
	static std::string StateToString(ELoadSaveState State );
	FSaveSlotListWrapper& GetSaveSlotWrapper();
	void BroadcastSucceeded();
	void BroadcastError();

	Json::Value ToJSon(SerializedDataPtr pData);
	SerializedDataPtr  FromJSon(Json::Value json);
	bool HasFakeLatency();
	SaveSlotListArg GetDungeonsCloudCharacters();
	APlayerController* GetPlayerController();
	static UDungeonsCloudSaveBPProxy* GetBP();

	void HandleASyncResult();
	SerializedDataPtr GetDungeonsFileContents(FStringArg FileName);

	void LoadDungeonsLocalArchive();
	void LoadDungeonsCloudArchives();
	uint32 GetDungeonsCloudFileSaveIndex(FStringArg FileName);
	FString GetNewDungeonsCloudFileName();
	uint32 GetDungeonsLatestFileSaveIndex();
	static FString GenerateDungeonsCloudFileName(uint32 Index);
	void SaveDungeonsCloudFile(FStringArg Name, SerializedDataPtr pData);
	void DeleteCloudFile(FStringArg Name);
	static bool IsDungeonsArchive(FStringArg FileName);

	void IncrementAsyncErrors();
	void HandleErrors();
	bool HadAnyErrors() const;
	void  ClearErrors();
	

	void SetState(ELoadSaveState s);
	bool IsUserAccountLinked();
	FUniqueNetIdArg GetNetId();
	void DeleteAllLocalfiles();
	void DeleteAllCloudFiles();
	void DeleteAllCloudFilesExcept(FStringArg f);
	TArray<FString> GetDungeonsDownloadFileList();
	static FCloudFileUtil* GetFileUtil();
	CloudHeadersArray GetUserFileList();
	SystemId GetSystemId();

	// Members

	ECloudOperation		mCloudOperation = ECloudOperation::Unknown;
	ELoadSaveState		mCloudLoadSaveState = ELoadSaveState::Idle;
	FOnlineStopWatch	mStopWatch;

	TArray<FString>		mFilesToDecompress;
	SerializedDataPtr	mpCompressedCloudArchive;



	std::unique_ptr<FCloudArchive>			mpArchive;
	TWeakObjectPtr<UDungeonsLocalPlayer>	mpLocalPlayer;

	int mAsyncErrors = 0;
	std::future<bool>	mAsyncResult;
	FString				mCurrentCloudFileName;
#if CLOUD_WANTDEBUGSTUFF
	int					mFakeLatency = 0;
#endif
	int					mCloudEnumStartDelay = 0;


};

class FDungeonsCloudLoadSave
{
public:
	FDungeonsCloudLoadSave();
	~FDungeonsCloudLoadSave();

	static void Init();
	static void Shutdown();


	static FOnlineLogger* GetLogger();
	static ZLibUtil* GetCompressor();
	static FCloudFileUtil* GetFileUtil();
	static FDungeonsCloudLoadSave* Instance();
	static UDungeonsCloudSaveBPProxy* GetBP();
	static	FDateTime TimeNow();
	void SetDungeonsCloudCharacters(SaveSlotListArg);
	void GetDungeonsCloudCharacters(UDungeonsCloudSaveBPProxy * pUDungeonsCloudSaveBP, SystemId localUser);
	void DownloadDungeonsCloudCharacter(APlayerCharacterSaveSlot* pSlot);
	void Tick();
	void CloudSaveExit( bool bSanityCheck );
	void CloudTitleScreenKickback();
	FSaveSlotListWrapper& GetSaveSlotWrapper();
private:
	FDungeonsCloudLoadSaveContext* GetContextFromNetId(FUniqueNetIdArg UserId);
	FDelegateHandle		mTickTimerHandle;
	std::shared_ptr<FDungeonsCloudLoadSaveContext> mpContext;
	FSaveSlotListWrapper mSaveSlotWrapper;

	int mFrame = 0;

	static TWeakObjectPtr<UDungeonsCloudSaveBPProxy> spUDungeonsCloudSaveBP;
	static std::unique_ptr<FOnlineLogger>	spLogger;
	static std::unique_ptr<ZLibUtil>		spCompressor;
	static std::unique_ptr<FCloudFileUtil>	spFileUtil;
	static std::unique_ptr<FDungeonsCloudLoadSave>	spInstance;
};

#endif