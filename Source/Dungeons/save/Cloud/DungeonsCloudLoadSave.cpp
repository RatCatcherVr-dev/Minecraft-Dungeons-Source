#include "Dungeons.h"
#include "..\..\save\DungeonsConsoleSave.h"
#include "PlatformFeatures.h"
#include "CloudfileUtil.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "CloudArchive.h"
#include "DungeonsUserManagement.h"
#include "DungeonsCloudSaveBP.h"
#include "game/actor/character/player/PlayerCharacterSaveSlot.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/UserCloud.h"
#include "game/skins/SkinsUtil.h"

#if CLOUDSAVE_ENABLED


static const int CLOUDSAVE_TICK_FREQUENCY = 60;
static const FString DUNGEONSCLOUDSAVE_ARCHIVE_NAME = FString("CloudSave");

std::unique_ptr<FDungeonsCloudLoadSave> FDungeonsCloudLoadSave::spInstance;
std::unique_ptr<FOnlineLogger> FDungeonsCloudLoadSave::spLogger;
std::unique_ptr<ZLibUtil>	FDungeonsCloudLoadSave::spCompressor;
std::unique_ptr<FCloudFileUtil>	FDungeonsCloudLoadSave::spFileUtil;
TWeakObjectPtr<UDungeonsCloudSaveBPProxy> FDungeonsCloudLoadSave::spUDungeonsCloudSaveBP;

static bool IsExitingGame()
{
	return GIsRequestingExit;
}

template<class T>
static bool IsFutureReady(T& Future)
{
	check(Future.valid());
	auto status = Future.wait_for(std::chrono::nanoseconds(1));
	return  status == std::future_status::ready;
}

static online::Crossplay::UserCloud* GetUserCloudInterface()
{
	return online::getCrossplayOss()->GetUserCloudIF().Get();
}

static bool HasCommandLine(const char* pCommand)
{
	return FOnlineUserCloudUtil::HasCommandLine(pCommand);
}


FDungeonsCloudLoadSave::FDungeonsCloudLoadSave()
{
	check(spLogger == nullptr);
	spLogger = std::make_unique<FOnlineLogger>("UserCloud");
	spCompressor = std::make_unique<ZLibUtil>(7);
	spFileUtil = FCloudFileUtil::CreateInstance();

	USERCLOUD_LOGFUNCTION;
	mTickTimerHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float time) -> bool {
		Instance()->Tick();
		return true;
	}), 1.0f / CLOUDSAVE_TICK_FREQUENCY);

}

FDungeonsCloudLoadSave::~FDungeonsCloudLoadSave()
{
	USERCLOUD_LOGFUNCTION;
	check(mpContext == nullptr);
	FTicker::GetCoreTicker().RemoveTicker(mTickTimerHandle);
	spFileUtil = nullptr;
	spCompressor = nullptr;
	spLogger = nullptr;
}


void FDungeonsCloudLoadSave::Init()
{
	if(spInstance == nullptr) {
		spInstance = std::make_unique<FDungeonsCloudLoadSave>();
	}
}

void FDungeonsCloudLoadSave::Shutdown()
{
	if(spInstance) {
		spInstance->CloudSaveExit(false);
		spInstance = nullptr;
	}
}

FDungeonsCloudLoadSave* FDungeonsCloudLoadSave::Instance()
{
	auto pNaked = spInstance.get();
	check(pNaked);
	return pNaked;

}

void FDungeonsCloudLoadSave::CloudTitleScreenKickback()
{
	CloudSaveExit(false);
}

FDateTime FDungeonsCloudLoadSave::TimeNow()
{
	return FDateTime::UtcNow();
}

FOnlineLogger* FDungeonsCloudLoadSave::GetLogger()
{
	check(spLogger.get());
	return spLogger.get();
}

ZLibUtil* FDungeonsCloudLoadSave::GetCompressor()
{
	check(spCompressor.get());
	return spCompressor.get();
}


FCloudFileUtil* FDungeonsCloudLoadSave::GetFileUtil()
{
	check(spFileUtil.get());
	return spFileUtil.get();
}

UDungeonsCloudSaveBPProxy* FDungeonsCloudLoadSave::GetBP()
{
	if (IsExitingGame())
		return nullptr;
	if (!spUDungeonsCloudSaveBP.IsValid())
		return nullptr;
	return spUDungeonsCloudSaveBP.Get();
}

void FDungeonsCloudLoadSave::CloudSaveExit( bool bSanityCheck)
{
	USERCLOUD_LOGFUNCTION;
	if (mpContext)
	{
		while (true)
		{
			if (mpContext->AreLocalFileOperationsCompleted())
				break;
			FPlatformProcess::Sleep(0.1f);
		}

		if ( bSanityCheck )
			mpContext->SanityCheck();
		mpContext = nullptr;
	}

}
void FDungeonsCloudLoadSave::DownloadDungeonsCloudCharacter(APlayerCharacterSaveSlot* pSlot)
{
	mpContext->DownloadDungeonsCloudCharacter(pSlot);
}

void FDungeonsCloudLoadSave::GetDungeonsCloudCharacters(UDungeonsCloudSaveBPProxy * pUDungeonsCloudSaveBP, SystemId localUser )
{
	USERCLOUD_LOG("%s : %d", __FUNCTION__, localUser);

	if (mpContext)
	{
		CloudSaveExit(true);
	}

	spUDungeonsCloudSaveBP = pUDungeonsCloudSaveBP;
	check(mpContext == nullptr);
	mpContext = std::make_shared<FDungeonsCloudLoadSaveContext>(localUser);
	mpContext->Sync();
}

void FDungeonsCloudLoadSave::SetDungeonsCloudCharacters(SaveSlotListArg List)
{
	mpContext->SetDungeonsCloudCharacters(List);
}

FDungeonsCloudLoadSaveContext* FDungeonsCloudLoadSave::GetContextFromNetId(FUniqueNetIdArg)
{
	auto pNaked = mpContext.get();
	check(pNaked);
	return pNaked;
}

void FDungeonsCloudLoadSave::Tick()
{
	mFrame++;
	if (mpContext)
	{
		auto pContext = mpContext;
		pContext->Tick();
	}
}

FSaveSlotListWrapper& FDungeonsCloudLoadSave::GetSaveSlotWrapper()
{
	return mSaveSlotWrapper;
}

FSaveSlotListWrapper& FDungeonsCloudLoadSaveContext::GetSaveSlotWrapper()
{
	return FDungeonsCloudLoadSave::Instance()->GetSaveSlotWrapper();
}


UGlobalStateData* FDungeonsCloudLoadSaveContext::GetGlobalSaveData()
{
	auto pController = GetPlayerController();
	auto pBaseController = (APlayerControllerBase*)pController;
	auto pGlobalStateData = pBaseController->EditGlobalSaveState();
	return pGlobalStateData;
}

FDungeonsCloudLoadSaveContext::FDungeonsCloudLoadSaveContext(SystemId Id)
{
	USERCLOUD_LOGUSERFUNCTION;

	mpArchive = std::make_unique < FCloudArchive >();
	auto LocalPlayerControllers = UDungeonsUserManager::Instance()->GetLocalMenuPlayerControllers();
	for (auto pLocalPlayerController : LocalPlayerControllers)
	{
		auto pLocalPlayer = pLocalPlayerController->GetLocalPlayer();
		auto pDungeonsPlayer = Cast<UDungeonsLocalPlayer>(pLocalPlayer);
		if (pDungeonsPlayer->GetSystemUserId() == Id)
		{
			mpLocalPlayer = pDungeonsPlayer;
			break;
		}
	}
	check(GetLocalPlayer());
	GetFileUtil()->SetFilePath(GetSystemId(), GetGlobalSaveData()->GetCharacterSavePath(GetSystemId()));
}

FDungeonsCloudLoadSaveContext::~FDungeonsCloudLoadSaveContext()
{
	if (IsUserAccountLinked())
		GetUserCloudInterface()->ClearFiles(GetNetId());
	GetSaveSlotWrapper().Free();
}

UDungeonsCloudSaveBPProxy* FDungeonsCloudLoadSaveContext::GetBP()
{
	return FDungeonsCloudLoadSave::GetBP();
}

void FDungeonsCloudLoadSaveContext::Sync()
{
	USERCLOUD_LOGUSERFUNCTION;

	check(mCloudOperation == ECloudOperation::Unknown);
	mCloudOperation = ECloudOperation::GetCharacters;

	if (IsUserAccountLinked())
	{
		SetState(ELoadSaveState::WaitingForLogin);
		GetUserCloudInterface()->Login(GetNetId(), [&](bool success) {

			if (success)
			{
				SetState(ELoadSaveState::StartEnumaratingCloudFilesDelay);
			}
			else
			{
				SetState(ELoadSaveState::UserAccountNotLinked);
				BroadcastError();
			}
		
		});
	}
	else
	{
		SetState(ELoadSaveState::UserAccountNotLinked);
		BroadcastError();
	}

}

void FDungeonsCloudLoadSaveContext::SaveDungeonsCloudFile(FStringArg Name, SerializedDataPtr pData)
{
	USERCLOUD_LOGUSERFUNCTION;
	GetUserCloudInterface()->WriteUserFile(GetNetId(), Name, *pData);
}

void FDungeonsCloudLoadSaveContext::DeleteCloudFile(FStringArg Name)
{
	USERCLOUD_LOGUSERFUNCTION;
	GetUserCloudInterface()->DeleteUserFile(GetNetId(), Name);
}

void FDungeonsCloudLoadSaveContext::SanityCheck()
{
	check(HaveAllOperationsCompleted());
	check(IsIdle() || IsErrored());
}

bool FDungeonsCloudLoadSaveContext::HaveAllOperationsCompleted()
{
	if (mAsyncResult.valid())
	{
		if (!IsFutureReady(mAsyncResult))
			return false;
	}
	return  AreCloudFileOperationsCompleted() && AreLocalFileOperationsCompleted();
}

uint32 FDungeonsCloudLoadSaveContext::GetDungeonsLatestFileSaveIndex()
{
	auto CloudFiles = GetUserFileList();
	uint32 Latest = 0;
	for (auto& f : CloudFiles)
		Latest = std::max(Latest, GetDungeonsCloudFileSaveIndex(f.FileName));
	return Latest;
}

FString FDungeonsCloudLoadSaveContext::GetNewDungeonsCloudFileName()
{
	auto NewIndex = GetDungeonsLatestFileSaveIndex() + 1;
	return GenerateDungeonsCloudFileName(NewIndex);
}

FString FDungeonsCloudLoadSaveContext::GenerateDungeonsCloudFileName(uint32 Index)
{
	return DUNGEONSCLOUDSAVE_ARCHIVE_NAME + FString(std::to_string(Index).c_str());
}

bool FDungeonsCloudLoadSaveContext::IsDungeonsArchive(FStringArg FileName)
{
	return FileName.Find(DUNGEONSCLOUDSAVE_ARCHIVE_NAME) == 0;
}

uint32 FDungeonsCloudLoadSaveContext::GetDungeonsCloudFileSaveIndex(FStringArg FileName)
{
	return IsDungeonsArchive(FileName) ?FCString::Atoi(*FileName.Mid(DUNGEONSCLOUDSAVE_ARCHIVE_NAME.Len())) : 0;
}

void FDungeonsCloudLoadSaveContext::HandleASyncResult()
{
	if (mAsyncResult.valid())
	{
		check(IsFutureReady(mAsyncResult));
		auto bSuccessful = mAsyncResult.get();
		if (!bSuccessful)
			IncrementAsyncErrors();
		check(!mAsyncResult.valid());
	}
}

void FDungeonsCloudLoadSaveContext::IncrementAsyncErrors()
{
	mAsyncErrors++;
}

bool FDungeonsCloudLoadSaveContext::HasFakeLatency()
{
#if CLOUD_WANTDEBUGSTUFF
	if (HasCommandLine("CloudFakeLatency"))
	{
		const int SecondsOfLatency = 4;
		if (mFakeLatency == 0)
			mFakeLatency = SecondsOfLatency * CLOUDSAVE_TICK_FREQUENCY;
		mFakeLatency--;
		return mFakeLatency != 0;
	}
#endif
	return false;
}

bool FDungeonsCloudLoadSaveContext::IsIdle() const
{
	return mCloudLoadSaveState == ELoadSaveState::Idle;
}

bool FDungeonsCloudLoadSaveContext::IsErrored() const
{
	return  mCloudLoadSaveState == ELoadSaveState::Error;
}

void FDungeonsCloudLoadSaveContext::Tick()
{
	if (IsExitingGame())
		return;
	if (!HaveAllOperationsCompleted())
		return;
	HandleASyncResult();
	HandleErrors();
	if (HasFakeLatency())
		return;

	switch (mCloudLoadSaveState)
	{
	case ELoadSaveState::Idle:
	case ELoadSaveState::Error:
		break;

	case ELoadSaveState::WaitingForLogin:
	{
		//Just wait
		mCloudEnumStartDelay = 0;
		break;
	}
	case ELoadSaveState::StartEnumaratingCloudFilesDelay:
	{
		//Theres some timing issue which means we have to wait here after logging in.
		if (mCloudEnumStartDelay > 30)
		{
			SetState(ELoadSaveState::StartEnumaratingCloudFiles);
		}
		++mCloudEnumStartDelay;

		break;
	}
	case ELoadSaveState::StartEnumaratingCloudFiles:
	{
		SetState(ELoadSaveState::WaitingForCloudFilesToBeEnumarated);
		GetUserCloudInterface()->EnumerateUserFiles(GetNetId());
		break;
	}

	case ELoadSaveState::WaitingForCloudFilesToBeEnumarated:
	{
#if CLOUD_WANTDEBUGSTUFF
		static bool sbOnce = false;
		if (!sbOnce)
		{
			sbOnce = true;
			if (HasCommandLine("CloudDeleteLocalFiles"))
				DeleteAllLocalfiles();
			if (HasCommandLine("CloudDeleteCloudFiles"))
			{
				SetState(ELoadSaveState::WaitingForCloudArchiveToBeDecompressed);
				break;
			}
		}
#endif
		if (HasCommandLine("CloudUseLocalFilesInsteadOfCloud"))
			LoadDungeonsLocalArchive();
		else
			LoadDungeonsCloudArchives();
		break;
	}
	case ELoadSaveState::WaitingForCloudFilesToBeRead:
	{
		mFilesToDecompress = GetDungeonsDownloadFileList();
		SetState(ELoadSaveState::DecompressNextFile);
		break;
	}
	case ELoadSaveState::DecompressNextFile:
	{
		if (mFilesToDecompress.Num())
		{
			mCurrentCloudFileName = mFilesToDecompress.Pop();
			auto pContents = GetDungeonsFileContents(mCurrentCloudFileName);
			mAsyncResult = std::async([&, pContents]()
			{
				return mpArchive->DecompressArchive(pContents);
			});
		}
		else
			mCurrentCloudFileName = "";
		SetState(ELoadSaveState::WaitingForCloudArchiveToBeDecompressed);
		break;
	}
	case ELoadSaveState::WaitingForCloudArchiveToBeDecompressed:
	{
		if (HadAnyErrors())
			SetState(ELoadSaveState::DecompressNextFile);
		else
		{
			DeleteAllCloudFilesExcept(mCurrentCloudFileName);
			mCurrentCloudFileName = GetNewDungeonsCloudFileName();
		}
		break;
	}
	case ELoadSaveState::WaitingForCloudArchiveToBeCompressed:
	{
		check(!mCurrentCloudFileName.IsEmpty());
		SaveDungeonsCloudFile(mCurrentCloudFileName, mpCompressedCloudArchive);
		SetState(ELoadSaveState::WaitingForCloudArchiveToBeUploaded);
		break;
	}
	case ELoadSaveState::WaitingForCloudArchiveToBeUploaded:
		DeleteAllCloudFilesExcept(mCurrentCloudFileName);
		break;
	case ELoadSaveState::WaitingForLocalArchiveToBeLoaded:
	case ELoadSaveState::WaitingForOldCloudArchivesToBeDeleted:
		BroadcastSucceeded();
		break;
	default:
		break;
	}
}

SerializedDataPtr FDungeonsCloudLoadSaveContext::GetDungeonsFileContents(FStringArg FileName)
{
	auto CloudInterface = GetUserCloudInterface();
	TArray<uint8> Contents;
	CloudInterface->GetFileContents(GetNetId(), FileName, Contents);
	return GetFileUtil()->MakeSerializedDataPtr(Contents);
}

void FDungeonsCloudLoadSaveContext::LoadDungeonsLocalArchive()
{
	USERCLOUD_LOGFUNCTION;

	SetState(ELoadSaveState::WaitingForLocalArchiveToBeLoaded);
	auto LocalFiles = std::make_shared< TArray<FString> >(GetFileUtil()->GetLocalFileList());
	mAsyncResult = std::async([&, LocalFiles]() {
		int FileIndex = 0;
		mpArchive->Clear();
		for (auto& F : *LocalFiles)
		{
			auto pData = GetFileUtil()->LoadLocalFile(F);
			if (pData == nullptr)
			{
				check(false);
				return false;
			}

			mpArchive->AddFile(FileIndex++, FCloudArchiveFile(FDungeonsCloudLoadSave::TimeNow(), pData));
		}
		return true;
	});
}

UDungeonsLocalPlayer* FDungeonsCloudLoadSaveContext::GetLocalPlayer()
{
	check(mpLocalPlayer.IsValid());
	return mpLocalPlayer.Get();
}

SystemId FDungeonsCloudLoadSaveContext::GetSystemId()
{
	return GetLocalPlayer()->GetSystemUserId();
}

#define STATE_TO_STRING(State)	case ELoadSaveState::State: return "ELoadSaveState::"#State;


std::string FDungeonsCloudLoadSaveContext::StateToString(ELoadSaveState State)
{
	switch (State)
	{
		STATE_TO_STRING(Idle);
		STATE_TO_STRING(WaitingForLogin);
		STATE_TO_STRING(StartEnumaratingCloudFilesDelay);
		STATE_TO_STRING(StartEnumaratingCloudFiles);
		STATE_TO_STRING(WaitingForCloudFilesToBeEnumarated);
		STATE_TO_STRING(WaitingForCloudFilesToBeRead);
		STATE_TO_STRING(DecompressNextFile);
		STATE_TO_STRING(WaitingForCloudArchiveToBeDecompressed);
		STATE_TO_STRING(WaitingForLocalArchiveToBeLoaded);
		STATE_TO_STRING(WaitingForCloudArchiveToBeCompressed);
		STATE_TO_STRING(WaitingForCloudArchiveToBeUploaded);
		STATE_TO_STRING(WaitingForOldCloudArchivesToBeDeleted);
		STATE_TO_STRING(UserAccountNotLinked);
		STATE_TO_STRING(Error);
	default:
		break;
	}

	check(false);
	return "ELoadSaveState::Unknwown";
}

void FDungeonsCloudLoadSaveContext::SetState(ELoadSaveState s)
{
	float DeltaTimeSeconds  = mStopWatch.GetElapsedSeconds();
	USERCLOUD_LOG("FDungeonsCloudLoadSaveContext::SetState : %s->%s Took : %.1f seconds",
		StateToString(mCloudLoadSaveState).c_str(),
		StateToString(s).c_str(),
		DeltaTimeSeconds
	);
	mCloudLoadSaveState = s;
}

bool FDungeonsCloudLoadSaveContext::IsUserAccountLinked()
{
	return GetLocalPlayer()->GetUniqueNetIdFromCachedControllerId().IsValid();
}

//static int GetPlayerxxxx APlayerCharacterSaveSlot* pSlot

void FDungeonsCloudLoadSaveContext::DownloadDungeonsCloudCharacter(APlayerCharacterSaveSlot* pSlot)
{
	USERCLOUD_LOGFUNCTION;

	auto pGlobalSaveData = GetGlobalSaveData();
	auto pSaveData = pSlot->GetCharacterSerializeComponent()->GetSaveData();
	auto DownloadCloudId = pSaveData->GetPlayerId();
	bool bAlreadyDownloaded = false;
	for (auto pSaveSlot : pGlobalSaveData->mSlots)
	{
		auto ThisSlotCloudId = pSaveSlot->GetPlayerId();
		bAlreadyDownloaded = ThisSlotCloudId == DownloadCloudId;
		if (bAlreadyDownloaded)
			break;
	}

	pSlot->SetCloudUploadeDateTime(FDungeonsCloudLoadSave::TimeNow());
	auto pController = GetPlayerController();
	auto pBaseController = (APlayerControllerBase*)pController;

	auto SavedCosmetics = pSaveData->mRecordedData.cosmetics;
	auto ThisSkin = pSlot->GetCharacterSkinId_Implementation();
	pSlot->StripDLCNotEntitled();
	if (bAlreadyDownloaded)
		pSaveData->SetNewPlayerId();
	pBaseController->DeepCloneCharacter(pSaveData);
	pSaveData->SetPlayerId(DownloadCloudId);
	pSlot->ChangeSkinId(ThisSkin);
	pSaveData->mRecordedData.cosmetics = SavedCosmetics;
}

FUniqueNetIdArg FDungeonsCloudLoadSaveContext::GetNetId()
{
	check(IsUserAccountLinked());
	return *GetLocalPlayer()->GetUniqueNetIdFromCachedControllerId();
}

void FDungeonsCloudLoadSaveContext::DeleteAllLocalfiles()
{
	auto LocalFiles = GetFileUtil()->GetLocalFileList();
	IFileManager& FileManager = IFileManager::Get();
	for (auto f : LocalFiles)
		GetFileUtil()->DeleteLocalFile(f);
}

void FDungeonsCloudLoadSaveContext::LoadDungeonsCloudArchives()
{
	USERCLOUD_LOGUSERFUNCTION;
	auto& UserId = GetNetId();
	auto CloudFiles = GetDungeonsDownloadFileList();
	if (CloudFiles.Num() != 0)
	{
		auto pInterface = GetUserCloudInterface();
		for (auto& f : CloudFiles)
			pInterface->ReadUserFile(UserId, f);
	}
	SetState(ELoadSaveState::WaitingForCloudFilesToBeRead);
}

TArray<FString> FDungeonsCloudLoadSaveContext::GetDungeonsDownloadFileList()
{
	TArray<FString> r;
	auto CloudFileList = GetUserFileList();
	for (auto& F : CloudFileList)
	{
		auto& FileName = F.FileName;
		if (!IsDungeonsArchive(FileName))
			continue;
		r.Add(F.FileName);
	}
	GetFileUtil()->PostProccessFileList(r);
	return r;
}

FCloudFileUtil* FDungeonsCloudLoadSaveContext::GetFileUtil()
{
	return FDungeonsCloudLoadSave::GetFileUtil();
}

CloudHeadersArray FDungeonsCloudLoadSaveContext::GetUserFileList()
{
	USERCLOUD_LOGUSERFUNCTION;
	return GetUserCloudInterface()->GetUserFileList(GetNetId());
}

void FDungeonsCloudLoadSaveContext::DeleteAllCloudFilesExcept(FStringArg Exception)
{
	SetState(ELoadSaveState::WaitingForOldCloudArchivesToBeDeleted);
#if CLOUD_WANTDEBUGSTUFF
	if (HasCommandLine("CloudRetainOldFiles"))
		return;
#endif
	auto CloudFiles = GetUserFileList();
	for (auto& f : CloudFiles)
	{
		if (f.FileName != Exception)
			DeleteCloudFile(f.FileName);
	}
}

void FDungeonsCloudLoadSaveContext::DeleteAllCloudFiles()
{
	USERCLOUD_LOGUSERFUNCTION;
	DeleteAllCloudFilesExcept("");
}

APlayerController* FDungeonsCloudLoadSaveContext::GetPlayerController()
{
	auto pPlayerController = GetLocalPlayer()->GetPlayerController(nullptr);
	check(pPlayerController != nullptr);
	return pPlayerController;
}

void FDungeonsCloudLoadSaveContext::HandleErrors()
{
	check(HaveAllOperationsCompleted());
	if (!HadAnyErrors())
		return;
	switch (mCloudLoadSaveState)
	{
	case ELoadSaveState::Idle:
	case ELoadSaveState::Error:
	case ELoadSaveState::WaitingForCloudArchiveToBeDecompressed:
	case ELoadSaveState::DecompressNextFile:
	//case ELoadSaveState::WaitingForCloudFilesToBeRead:
	case ELoadSaveState::WaitingForOldCloudArchivesToBeDeleted:
		break;
	default:
		BroadcastError();
		break;
	}
}

void FDungeonsCloudLoadSaveContext::BroadcastSucceeded()
{
	USERCLOUD_LOGFUNCTION;
	ClearErrors();
	SetState(ELoadSaveState::Idle);
	switch (mCloudOperation)
	{
	case ECloudOperation::GetCharacters:	GetBP()->GetCloudCharactersCompleted.Broadcast(GetDungeonsCloudCharacters(), ECloudCharactersResult::Success);	break;
	case ECloudOperation::SetCharacters:	GetBP()->SetCloudCharactersCompleted.Broadcast(GetSaveSlotWrapper().Get(), ECloudCharactersResult::Success);	break;
	default:								check(false); break;
	}
	mCloudOperation = ECloudOperation::Unknown;
}

void FDungeonsCloudLoadSaveContext::BroadcastError()
{
	USERCLOUD_LOGFUNCTION;
//	CLOUD_NOT_IMPLEMENTED_YET;
	check(mCloudOperation != ECloudOperation::Unknown);
	auto ErrorCode = ECloudCharactersResult::Failed;
	SaveSlotList Dummy;
	ClearErrors();
	switch (mCloudLoadSaveState)
	{
	case ELoadSaveState::UserAccountNotLinked:	ErrorCode = ECloudCharactersResult::FailedAccountNotLinked;	break;
	case ELoadSaveState::WaitingForLogin:		ErrorCode = ECloudCharactersResult::FailedLogin;			break;
	default:									break;
	}
	SetState(ELoadSaveState::Error);
	switch (mCloudOperation)
	{
	case ECloudOperation::GetCharacters:	GetBP()->GetCloudCharactersCompleted.Broadcast(Dummy,ErrorCode);	break;
	case ECloudOperation::SetCharacters:	GetBP()->SetCloudCharactersCompleted.Broadcast(Dummy, ErrorCode);	break;
	default:								check(false); break;
	}
	mCloudOperation = ECloudOperation::Unknown;
}

bool	FDungeonsCloudLoadSaveContext::AreCloudFileOperationsCompleted() const
{
	return GetUserCloudInterface()->AreCloudFileOperationsCompleted();
}

bool FDungeonsCloudLoadSaveContext::AreLocalFileOperationsCompleted() const
{
	return GetFileUtil()->AreLocalFileOperationsCompleted();
}

void  FDungeonsCloudLoadSaveContext::ClearErrors()
{
	check(HaveAllOperationsCompleted());
	mAsyncErrors = 0;
	GetUserCloudInterface()->ClearErrors();
}

bool FDungeonsCloudLoadSaveContext::HadAnyErrors() const
{
	return GetUserCloudInterface()->HadAnyErrors() || mAsyncErrors != 0 ;
}

Json::Value FDungeonsCloudLoadSaveContext::ToJSon(SerializedDataPtr pData)
{
	Json::Value root;
	Json::Reader reader;
	const char *begining = (const char*)pData->GetData();
	const char *end = begining + pData->Num();

	if (!reader.parse(begining, end, root))
	{
		check(false);
		return {};
	}
	return root;
}

SerializedDataPtr  FDungeonsCloudLoadSaveContext::FromJSon(Json::Value node)
{
	SerializedDataPtr pData;

	Json::FastWriter writer;
	std::string json = writer.write(node);

	return GetFileUtil()->MakeSerializedDataPtr(json);
}

SaveSlotListArg FDungeonsCloudLoadSaveContext::GetDungeonsCloudCharacters()
{
	check(IsIdle());
	//check(GetSaveSlotWrapper.Get().Num() == 0);
	GetSaveSlotWrapper().Free();
	SaveSlotList  r;
	auto WorldContextObject = GetPlayerController();
	auto pWorld = WorldContextObject->GetWorld();

	for (int i = 0; i != mpArchive->GetNumberOfFiles(); i++)
	{
		check(mpArchive->DoesFileExist(i));
		auto File = mpArchive->GetFile(i);
		APlayerCharacterSaveSlot* spawnedSlot = pWorld->SpawnActor<APlayerCharacterSaveSlot>();
		spawnedSlot->IgnoreEntitlements();
		auto saveData = File.mpData;
		auto DateTime = File.mDateTime;
		spawnedSlot->SetCloudUploadeDateTime(DateTime);
		UCharacterSaveData* toby = NewObject<UCharacterSaveData>(WorldContextObject);
		toby->CreateNew("CLOUDFILE");
		auto jSon = ToJSon(saveData);
		toby->AfterLoadSaveData(GetSystemId(), jSon);
		spawnedSlot->AssignSaveData(toby);
		
		r.Add(spawnedSlot);

	}
	GetSaveSlotWrapper().Set(r);
	return GetSaveSlotWrapper().Get();
}

void FDungeonsCloudLoadSaveContext::SetDungeonsCloudCharacters(SaveSlotListArg CloudCharacters)
{
	USERCLOUD_LOGFUNCTION;

	check(mCloudOperation == ECloudOperation::Unknown);
	mCloudOperation = ECloudOperation::SetCharacters;
	mpArchive->Clear();

	if (CloudCharacters == GetSaveSlotWrapper().Get())
	{
		BroadcastSucceeded();
		return;
	}
	mAsyncResult = std::async([&, CloudCharacters]()
	{
		for (int i = 0; i != CloudCharacters.Num(); i++)
		{
			auto pSaveSlot = CloudCharacters[i];
			check(pSaveSlot);
			auto pCharacterSerializeComponent = pSaveSlot->GetCharacterSerializeComponent();
			UCharacterSaveData* pSaveData = pCharacterSerializeComponent->GetSaveData();
			auto& RecordedData = pSaveData->mRecordedData;
			Json::Value node;
			RecordedData.RecordSaveDataToJsonNode(node);
			auto pData = FromJSon(node);
			auto DateTime = pSaveSlot->GetCloudUploadeDateTime();
			mpArchive->AddFile(i, FCloudArchiveFile(DateTime, pData));
		}
		mpCompressedCloudArchive = mpArchive->CompressArchive();
		return true;
	});
	SetState(ELoadSaveState::WaitingForCloudArchiveToBeCompressed);
}

FSaveSlotListWrapper::FSaveSlotListWrapper()
{
	mList.Reserve(FCloudArchive::CLOUDARCHIVE_MAXFILES);
}

FSaveSlotListWrapper::~FSaveSlotListWrapper()
{
	Free();
}

SaveSlotListArg FSaveSlotListWrapper::Get()
{
	return mList;
}

void FSaveSlotListWrapper::Set(SaveSlotListArg List)
{
	Free();
	mList = List;
}
void FSaveSlotListWrapper::Free()
{
	mList.Reset();
}


#endif