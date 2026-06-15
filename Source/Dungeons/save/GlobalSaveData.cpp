#include "GlobalSaveData.h"
#include "JsonCommon.h"
#include "game/SaveConstants.h"
#include "util/StringUtil.h"
#include "Kismet/KismetGuidLibrary.h"
#include "save/DungeonsConsoleSave.h" //D11.PS
#include "DungeonsUserManagement.h"
#include "DungeonsGameInstance.h"
#include "SaveSpinnerInterface.h"
#include "online/sessions/OnlineUtil.h"
#include "DungeonsSaveVersion.h"
#include "BinarySaveData.h"
#include "online/crossplay/Identity.h"
#include "online/crossplay/UserCloud.h"
#include "world/entity/EntityTypes.h"
#include "online/entitlements/Entitlement.h"

#if !CONSOLE_SAVE_SYSTEM
#include "Windows/AllowWindowsPlatformTypes.h"
#include <shellapi.h>
#include <ShlObj.h>

#endif

static auto CloudNagKey = "CloudNagShown";
static bool sgbCloudNag = false;

const char *TrackedStatsKey = "trackedStats";


static int sSeenNewsPruneThreshold = 10; //We only need to keep a limited history of this - and keeping an infinite history could be associated with stability concerns in the long-term.
FAutoConsoleVariableRef CVarSeenNewsPruneThreshold(
	TEXT("Dungeons.News.SeenNewsPruneThreshold"),
	sSeenNewsPruneThreshold,
	TEXT("Maximum number previously seen news values to record in the global save data"),
	ECVF_Default);


static void SetDisplayCloudServicesFeature(bool bNag)
{
	sgbCloudNag = bNag;
}
bool ShouldDisplayCloudServicesFeature()
{
	bool bShow = sgbCloudNag;
	if (bShow)
		sgbCloudNag = false;
	return bShow;
}

#define LOCTEXT_NAMESPACE "Save_Data_Global"

namespace json { namespace global {

ReconnectSaveData parseReconnect(const Json::Value& node) {
	ReconnectSaveData r;
	r.sessionId = FString(node["sessionId"].asString().c_str());

	bool success = false;
	UKismetGuidLibrary::Parse_StringToGuid(stringutil::toFString(node["guid"].asString()), r.guid, success);

	return r;
}

Json::Value toJson(const ReconnectSaveData& reconnect) {
	Json::Value node;
	node["sessionId"] = stringutil::toStdString(reconnect.sessionId);
	node["guid"] = stringutil::toStdString(reconnect.guid.ToString());
	return node;
}

TArray<FEntitlement> parseCachedEntitlements(const Json::Value& root) {
	TArray<FEntitlement> entitlements;
	for (auto& entry : root) {
		entitlements.Add(FEntitlement(
			entry["name"].asString().c_str(),
			entry["signature"].asString().c_str(),
			entry["signingUserId"].asString().c_str(),
			EEntitlementsSource::CachedLocally
		));
	}
	return entitlements;
}

Json::Value toJson(const TArray<FEntitlement>& cachedEntitlements) {
	Json::Value node;
	for (auto& entitlement : cachedEntitlements) {
		Json::Value itemNode;
		itemNode["name"] = stringutil::toStdString(entitlement.GetName());
		itemNode["signature"] = stringutil::toStdString(entitlement.GetSignature());
		itemNode["signingUserId"] = stringutil::toStdString(entitlement.GetSigningUserId());
		itemNode["entitlementsSource"] = Json::Value(static_cast<uint8>(entitlement.GetEntitlementsSource()));
		node.append(itemNode);
	}
	return node;
}

TMap<FString, FString> parseRecentFiles(const Json::Value& root) {
	TMap<FString, FString> map;
	if (root.isObject()) {
		std::vector<std::string> keys = root.getMemberNames();
		for (const auto& key : keys) {
			std::string value = root[key].asString();
			map.Add(stringutil::toFString(key), stringutil::toFString(value));
		}
	}

	return map;
}

Json::Value toJson(TMap<FString, FString> inMap) {
	Json::Value node;
	for (TPair<FString, FString> entry : inMap) {
		node[stringutil::toStdString(entry.Key)] = stringutil::toStdString(entry.Value);
	}

	return node;
}


// D11.DB - START
TArray<FKeybinding> parseKeybinds(const Json::Value& root) {
	TArray<FKeybinding> binds;
	for (auto& entry : root) {
		binds.Emplace(
			FName(entry["action"].asString().c_str()),
			FKey(*FString(entry["key"].asString().c_str())),
			UKeybindHelper::StringToKeybindPlatform(FString(entry["platform"].asString().c_str()))
		);
	}
	return binds;
}

Json::Value toJson(const TArray<FKeybinding>& inKeybinds) {
	Json::Value node;
	for (auto& item : inKeybinds) {
		Json::Value itemNode;
		itemNode["key"] = stringutil::toStdString(item.Key.ToString());
		itemNode["platform"] = stringutil::toStdString(UKeybindHelper::KeybindPlatformToString(item.Type));
		itemNode["action"] = stringutil::toStdString(item.Name.ToString());
		node.append(itemNode);
	}
	return node;
}
// D11.DB - END

// D11.SSN - Begin
TrackedStats parseTrackedStats(const Json::Value& root, uint32 version) {
	TrackedStats stats;
	if (version == 1) {
		stats.achievements = root["achievements"].asUInt64();
		stats.cachedAchievements = root["cachedAchievements"].asUInt64();
	}
	else {
		stats.achievements = std::bitset<128>(root["achievements"].asString());
		stats.cachedAchievements = std::bitset<128>(root["cachedAchievements"].asString());
	}
	stats.revives = root["revives"].asUInt();
	stats.mobKills = root["mobKills"].asUInt();
	stats.passiveKills = root["passiveKills"].asUInt();
	stats.enchantedKills = root["enchantedKills"].asUInt();
	stats.endlerlingKills = root["enderlingKills"].asUInt();
	stats.level = root["level"].asUInt();
	stats.chestsOpened = root["chestsOpened"].asUInt();
	stats.emeralds = root["emeralds"].asUInt();
	stats.foodEaten = root["foodEaten"].asUInt();
	stats.blocksSlid = root["blocksSlid"].asFloat();
	stats.wolf = root["wolf"].asBool();
	stats.llama = root["llama"].asBool();
	stats.golem = root["golem"].asBool();
	stats.pumpkinPastures = root["pumpkinPastures"].asBool();
	stats.soggySwamp = root["soggySwamp"].asBool();
	stats.redstoneMines = root["redstoneMines"].asBool();
	stats.cactiCanyon = root["cactiCanyon"].asBool();
	stats.desertTemple = root["desertTemple"].asBool();
	stats.fieryForge = root["fieryForge"].asBool();
	stats.highblockHalls = root["highblockHalls"].asBool();
	stats.obsidianPinnacle = root["obsidianPinnacle"].asBool();
	stats.timePlayed = root["timePlayed"].asFloat();
	stats.deaths = root["deaths"].asUInt();
	stats.distanceTravelled = root["distanceTravelled"].asFloat();
	stats.healed = root["healed"].asFloat();
	stats.usedArtefacts = root["usedArtefacts"].asUInt();
	stats.titles = std::bitset<128>(root["titles"].asString());
	stats.TNTKills = root["TNTKills"].asUInt();
	stats.creeperKills = root["creeperKills"].asUInt();
	stats.crimsonForest = root["crimsonForest"].asBool();
	stats.soulsandValley = root["soulsandValley"].asBool();
	stats.netherFortress = root["netherFortress"].asBool();
	stats.wolfHome = root["wolfHome"].asBool();
	stats.batHome = root["batHome"].asBool();
	stats.wraithHome = root["wraithHome"].asBool();
	stats.gold = root["gold"].asUInt();
	stats.bannerMissions = root["bannerMissions"].asUInt();
	stats.stronghold = root["stronghold"].asBool();
	stats.brokenCitadel = root["brokenCitadel"].asBool();
	for (Json::Value::ArrayIndex i = 0; i < root["ancientKills"].size(); i++) {
		stats.ancientKills.AddUnique(EntityTypeFromString(root["ancientKills"][i].asString()));
	}
	for (Json::Value::ArrayIndex i = 0; i < root["coopMissions"].size(); i++) {
		TPair<bool, TPair<int32, int32>> mission;
		mission.Key = root["coopMissions"][i]["missionType"].asBool();
		mission.Value.Key = root["coopMissions"][i]["year"].asInt();
		mission.Value.Value = root["coopMissions"][i]["month"].asInt();
		stats.coopMissions.Emplace(mission);
	}
	for (Json::Value::ArrayIndex i = 0; i < root["underwaterHooks"].size(); i++) {
		stats.underwaterHooks.AddUnique(EntityTypeFromString(root["underwaterHooks"][i].asString()));
	}
	return stats;
}

Json::Value toJson(const TrackedStats& inStats) {
	Json::Value node;
	node["achievements"] = Json::Value(inStats.achievements.to_string());
	node["cachedAchievements"] = Json::Value(inStats.cachedAchievements.to_string());
	node["revives"] = Json::Value(inStats.revives);
	node["mobKills"] = Json::Value(inStats.mobKills);
	node["passiveKills"] = Json::Value(inStats.passiveKills);
	node["enchantedKills"] = Json::Value(inStats.enchantedKills);
	node["enderlingKills"] = Json::Value(inStats.endlerlingKills);
	node["level"] = Json::Value(inStats.level);
	node["chestsOpened"] = Json::Value(inStats.chestsOpened);
	node["emeralds"] = Json::Value(inStats.emeralds);
	node["foodEaten"] = Json::Value(inStats.foodEaten);
	node["blocksSlid"] = Json::Value(inStats.blocksSlid);
	node["wolf"] = Json::Value(inStats.wolf);
	node["llama"] = Json::Value(inStats.llama);
	node["golem"] = Json::Value(inStats.golem);
	node["pumpkinPastures"] = Json::Value(inStats.pumpkinPastures);
	node["soggySwamp"] = Json::Value(inStats.soggySwamp);
	node["redstoneMines"] = Json::Value(inStats.redstoneMines);
	node["cactiCanyon"] = Json::Value(inStats.cactiCanyon);
	node["desertTemple"] = Json::Value(inStats.desertTemple);
	node["fieryForge"] = Json::Value(inStats.fieryForge);
	node["highblockHalls"] = Json::Value(inStats.highblockHalls);
	node["obsidianPinnacle"] = Json::Value(inStats.obsidianPinnacle);
	node["timePlayed"] = Json::Value(inStats.timePlayed);
	node["deaths"] = Json::Value(inStats.deaths);
	node["distanceTravelled"] = Json::Value(inStats.distanceTravelled);
	node["healed"] = Json::Value(inStats.healed);
	node["usedArtefacts"] = Json::Value(inStats.usedArtefacts);
	node["titles"] = Json::Value(inStats.titles.to_string());
	node["TNTKills"] = Json::Value(inStats.TNTKills);
	node["creeperKills"] = Json::Value(inStats.creeperKills);
	node["crimsonForest"] = Json::Value(inStats.crimsonForest);
	node["soulsandValley"] = Json::Value(inStats.soulsandValley);
	node["netherFortress"] = Json::Value(inStats.netherFortress);
	node["wolfHome"] = Json::Value(inStats.wolfHome);
	node["batHome"] = Json::Value(inStats.batHome);
	node["wraithHome"] = Json::Value(inStats.wraithHome);
	node["gold"] = Json::Value(inStats.gold);
	node["bannerMissions"] = Json::Value(inStats.bannerMissions);
	node["stronghold"] = Json::Value(inStats.stronghold);
	node["brokenCitadel"] = Json::Value(inStats.brokenCitadel);
	node["ancientKills"] = Json::arrayValue;
	for (EntityType entity : inStats.ancientKills) {
		node["ancientKills"].append(Json::Value(EntityTypeToString(entity)));
	}
	node["coopMissions"] = Json::arrayValue;
	for (TPair<bool, TPair<int32, int32>> mission : inStats.coopMissions) {
		Json::Value arrayNode;
		arrayNode["missionType"] = Json::Value(mission.Key);
		arrayNode["year"] = Json::Value(mission.Value.Key);
		arrayNode["month"] = Json::Value(mission.Value.Value);
		node["coopMissions"].append(arrayNode);
	}
	for (EntityType entity : inStats.underwaterHooks) {
		node["underwaterHooks"].append(Json::Value(EntityTypeToString(entity)));
	}
	return node;
}
}}
// D11.SSN - End



#if CONSOLE_SAVE_SYSTEM


//D11.SC - Async Json Save Implementation
class FAsyncGlobalStateDataJsonSave : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FAsyncGlobalStateDataJsonSave>;

public:
	FAsyncGlobalStateDataJsonSave(const UGlobalStateData::RecordedData& SourceData, int userIndex)
		:
		mGlobalSaveData(SourceData),
		mUserIndex(userIndex)
	{

	}

protected:
	UGlobalStateData::RecordedData mGlobalSaveData;
	int mUserIndex;

	void DoWork()
	{
		Json::Value node;
		mGlobalSaveData.RecordGlobalSaveDataToJsonNode(node);

		Json::FastWriter writer;
		std::string json= writer.write(node);

		// D11.SSN
		SaveMetaData metaData;
		metaData.saveName = CONSOLE_GLOBAL_SAVE_FILE;
		metaData.title = FText(LOCTEXT("Save_Global_Title", "Minecraft Dungeons - Profile & Settings")).ToString();
		metaData.subTitle = FString::Printf(TEXT("%s: %.2d:%.2d:%.2d"), *FText(LOCTEXT("Save_Global_SubTitle", "Time Played")).ToString(), int(mGlobalSaveData.mTrackedStats.timePlayed) / 60 / 60 / 24, int(mGlobalSaveData.mTrackedStats.timePlayed) / 60 % 60, int(mGlobalSaveData.mTrackedStats.timePlayed) % 60);
		metaData.details = FText(LOCTEXT("Save_Global_Detail", "Contains your profile, settings, and stat data. Do not delete.")).ToString();
#if PLATFORM_PS4
		metaData.iconPath = TEXT("../../../dungeons/Content/SaveIcons/save_global.png");
#endif
		DungeonsConsoleSave::Instance()->SaveAsync(metaData, mUserIndex, json);

	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncGlobalStateDataJsonSave, STATGROUP_ThreadPoolAsyncTasks);
	}
};


#endif// CONSOLE_SAVE_SYSTEM

static void ReplaceAndRemoveSaveFile(const FString& newPath, const FString& oldPath, const FString& filename) {
	FString oldFilePath = oldPath / filename;
	FString filePath = newPath / filename;

	if(!FPaths::FileExists(filePath))
	{
		auto oldSaveData = binarysavedata::loadBinaryFileToJson(oldFilePath);
		if (oldSaveData)
		{
			if (!IFileManager::Get().DirectoryExists(*newPath))
				IFileManager::Get().MakeDirectory(*newPath, true);
			if (!binarysavedata::writeJsonToBinaryFile(oldSaveData.GetValue(), filePath))
			{
				UE_LOG(LogDungeons, Error, TEXT("Unable to write new save file! Old save will be moved to '%s'. A blank save file will be created."), *(oldFilePath + ".bak"))
			}
		}

		//else // still disable the old file, to avoid perpetually hitting this code
		IFileManager::Get().Move(*(oldFilePath + ".bak"), *oldFilePath, true, true);

		if (!binarysavedata::loadBinaryFileToJson(filePath))
		{
			UE_LOG(LogDungeons, Error, TEXT("Unable to open save file, the backup save is in %s"), *(oldFilePath + ".bak"));
		}
	}
}




UGlobalStateData::UGlobalStateData() {
	USaveSpinnerInterface::InitialiseSaveSpinner();

}

UGlobalStateData::~UGlobalStateData()
{
}

void UGlobalStateData::CreateNewState() {
	mRecordedData.mInitialBootFlowComplete = false;
	mRecordedData.mReconnect.guid = UKismetGuidLibrary::NewGuid();
}

void UGlobalStateData::LoadAllCharacterProfiles(int32 localUserNum)
{



#if CONSOLE_SAVE_SYSTEM
	//D11.PS5
	TArray<FString> saveFiles;
	DungeonsConsoleSave::Instance()->ListSaves(localUserNum, saveFiles);

	bool hasCharacterSaves = false;
	for (FString& file : saveFiles)
	{
		if (file.Contains(ConsoleSave::SaveFileAppend))
		{
			hasCharacterSaves = true;
			CharacterSlotsLeftToCheck++;
			LoadCharacterSaveData(file, localUserNum);
		}
	}

	if (!hasCharacterSaves)
	{
		OnGlobalSaveDataLoaded.Broadcast(this, localUserNum);
	}
#else
	FString savePath = GetCharacterSavePath(localUserNum);
	IFileManager& FileManager = IFileManager::Get();

	// move over old save files, if any:
	FString oldSavePath = GetOldCharacterSavePath(localUserNum);
	TArray<FString> oldSaveFiles;
	FileManager.FindFiles(oldSaveFiles, *(oldSavePath / "*" + globalsavedata::BinaryFileExtension), true, false);
	oldSaveFiles = TSet<FString>(oldSaveFiles).Array();
	for (const FString& file : oldSaveFiles) {
		ReplaceAndRemoveSaveFile(savePath, oldSavePath, file);
	}

	TArray<FString> saveFiles;
	FileManager.FindFiles(saveFiles, *(savePath / "*" + globalsavedata::BinaryFileExtension), true, false);

	// In some cases (playing a Development build through Visual Studio) FileManager will return duplicate files. We havent' seen it in real builds.
	saveFiles = TSet<FString>(saveFiles).Array();

	CharacterSlotsLeftToCheck = saveFiles.Num();
	if (saveFiles.Num() > 0)
	{
		for (const FString& file : saveFiles) {
			LoadCharacterSaveData(savePath / file);
		}
	}
	else
	{
		OnGlobalSaveDataLoaded.Broadcast(this, localUserNum);
	}
#endif
}

FString UGlobalStateData::GetRecentSaveDataByKey(const FString& key) const {
	if (!key.IsEmpty()) {
		if (const FString* value = mRecordedData.mRecentSaveFilenames.Find(key)) {
			return *value;
		}
	}

	return {};
}

void UGlobalStateData::SetRecentSaveData(const FString& key, const FString& filename) {
	mRecordedData.mRecentSaveFilenames.Add(key, filename);
}

FString UGlobalStateData::GetProfilePath(uint32 currentPlayerIndex) const {
	FString savePath = "";

	// if nullsystem, the UID will be different every time we start.
	if (online::usingNullSubsystem(GetWorld())) {
		savePath += FString("offline-userid/");
	}
	else if (online::usingDungeonsSubsystem(GetWorld())) {
		// xuid independent path
		auto identityInterface = online::getIdentityInterface();
		if (identityInterface.IsValid()) {
			/*
				Our current local coop flow, has the issue that we do not know who the sponsor is.
				users cant sign in separately, or pick their sponsor.

				therefore, in order to move this along, if you're not player 0, your sponsor will be player 0.
			*/
			const uint32 pc_player_zero_is_always_the_sponsor = 0;
			TSharedPtr<const FUniqueNetId> xuid = identityInterface->GetUniquePlayerId(pc_player_zero_is_always_the_sponsor);
			if (xuid) {
				savePath += xuid->ToString() + stringutil::toFString("/");
			}
			// we've seen this happen starting another level that isn't following the login flow when starting through editor standalone.
#if WITH_EDITOR
			else {
				savePath += stringutil::toFString("offlineXuid/");
			}
#endif
		}
	}
#if WITH_EDITOR
	// create directory for player saves depending on PIE
	if (!mPIESaveFolder.empty()) {
		savePath += stringutil::toFString(mPIESaveFolder + "/");
	}
#else
#endif

	return savePath;
}

// lift in these windows-calls, as Unreal did not define support for FOLDERID_SavedGames
static const TCHAR* SaveFilesPath()
{
	static FString SavedGamesDir;
	if (!SavedGamesDir.Len())
	{
#if !CONSOLE_SAVE_SYSTEM
		TCHAR* SavedGamesPath;
		HRESULT Ret = SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &SavedGamesPath);
		if (SUCCEEDED(Ret))
		{
			// make the base user dir path
			SavedGamesDir = FString(SavedGamesPath).Replace(TEXT("\\"), TEXT("/")) + TEXT("/");
			CoTaskMemFree(SavedGamesPath);
		}
		else
		{
			UE_LOG(LogDungeons, Error, TEXT("Unable to get the Windows 'FOLDERID_SavedGames' Path. This can result in unfunctioning save files."));
		}
#else
	SavedGamesDir = FPlatformProcess::UserDir();
#endif // !CONSOLE_SAVE_SYSTEM
	}
	return *SavedGamesDir;
}

FString UGlobalStateData::GetGlobalSavePath(uint32 currentPlayerIndex) const {
	FString savePath = SaveFilesPath(); // Saved games dir
	savePath += FString("Mojang Studios") / FApp::GetProjectName() / GetProfilePath(currentPlayerIndex);
	return savePath;
}

FString UGlobalStateData::GetOldSavePath(uint32 currentPlayerIndex) const {
	FString savePath = FPlatformProcess::UserSettingsDir();	// Appdata Local path
	savePath += FApp::GetProjectName() / GetProfilePath(currentPlayerIndex);
	return savePath;
}

FString UGlobalStateData::GetCharacterSavePath(uint32 currentPlayerIndex) const {
	return GetGlobalSavePath(currentPlayerIndex) + "Characters/";
}

FString UGlobalStateData::GetOldCharacterSavePath(uint32 currentPlayerIndex) const {
	return GetOldSavePath(currentPlayerIndex) + "Characters/";
}

FString UGlobalStateData::GetDeletedCharacterSavePath(uint32 currentPlayerIndex) const {
	return GetGlobalSavePath(currentPlayerIndex) + "CharactersTrashcan/";
}

FString UGlobalStateData::GenerateCharacterFilename(uint32 currentPlayerIndex) const {
	FString savePath = GetCharacterSavePath(currentPlayerIndex);

#if CONSOLE_SAVE_SYSTEM
	// #D11.CM - Prepend console to the save path
	savePath = savePath / ConsoleSave::SaveFileAppend + FGuid::NewGuid().ToString() + ".json";

#else
	savePath = savePath / FGuid::NewGuid().ToString() + globalsavedata::BinaryFileExtension;
#endif

	return savePath;
}

bool UGlobalStateData::IsInitialBootFlowComplete() const
{
	return mRecordedData.mInitialBootFlowComplete;
}

void UGlobalStateData::SetInitialBootFlowComplete()
{
	mRecordedData.mInitialBootFlowComplete = true;
}

UCharacterSaveData* UGlobalStateData::CloneCharacterProfile(const UCharacterSaveData& cloneSource, int32 localUserNum , ECharacterCloneType CloneType) {
	FString savePath = GenerateCharacterFilename(localUserNum);

	UCharacterSaveData* dolly = NewObject<UCharacterSaveData>(this);
	if ( CloneType == ECharacterCloneType::Deep )
		dolly->CreateNewDeepClone(cloneSource, savePath);
	else
		dolly->CreateNewShallowClone(cloneSource, savePath);
	mSlots.Add(dolly);

	dolly->Save(localUserNum);

	return dolly;
}



UCharacterSaveData* UGlobalStateData::CreateNewCharacterProfile(int32 localUserNum) {
	FString savePath = GenerateCharacterFilename(localUserNum);

	UCharacterSaveData* toby = NewObject<UCharacterSaveData>(this);
	toby->CreateNew(savePath);
	toby->SetLegendaryStatus(mRecordedData.mTrackedStats.obsidianPinnacle);
	mSlots.Add(toby);
	toby->Save(localUserNum);

	return toby;
}


FString  UGlobalStateData::GetGlobalSaveFileName() const
{
#if CONSOLE_SAVE_SYSTEM
	return CONSOLE_GLOBAL_SAVE_FILE;
#else
	return "savefile" + globalsavedata::BinaryFileExtension;
#endif
}

void UGlobalStateData::Load(int32 localUserNum) {


#if CONSOLE_SAVE_SYSTEM
	//D11.PS - Console save data
	TArray<uint8> saveBlob;
	DungeonsConsoleSave::Instance()->IntegretyCheck(localUserNum);
	TArray<uint8> saveData;
	GlobalSaveDataLoadDelegate = DungeonsConsoleSave::Instance()->OnAsyncLoadFinished.AddUObject(this, &UGlobalStateData::AfterConsoleSaveDataLoaded);
	DungeonsConsoleSave::Instance()->Load(CONSOLE_GLOBAL_SAVE_FILE, localUserNum, saveData);

	return;
#else
	//PC Save
	//D11.KS - On PC local user num 0 is always the first player, other local players should always generate new global save data from default.
	//D11.GM - We want all local players to start with the first players save data
	//MJS.DG - Hi guys, i also miss IRC.
	//jryden : checking for old save data folder, if it exists: load that data and delete the folder.
	ReplaceAndRemoveSaveFile(GetGlobalSavePath(localUserNum), GetOldSavePath(localUserNum), GetGlobalSaveFileName());

	const FString savefilePath = GetGlobalSavePath(localUserNum) / GetGlobalSaveFileName();

	AddProfilesInPath(savefilePath, localUserNum);
#endif
}


void UGlobalStateData::AddProfilesInPath(const FString& inPath, int32 localUserNum) {
#if CONSOLE_SAVE_SYSTEM
	checkNoEntry();
#else
	if (auto maybeJson = binarysavedata::safer::loadBinaryFileToJsonSafer(inPath))
	{
		AfterLoadGlobalSaveData(maybeJson.GetValue(), localUserNum);
	}
	else
	{
		SetDisplayCloudServicesFeature(true);
		CreateNewState();
		Save(localUserNum);

		LoadAllCharacterProfiles(localUserNum);
	}
#endif
}


void UGlobalStateData::AfterConsoleSaveDataLoaded(bool bSuccessfull, FString filename, int32 localUserNum, TArray<uint8> &saveBlob)
{
#if CONSOLE_SAVE_SYSTEM
	DungeonsConsoleSave::Instance()->OnAsyncLoadFinished.Remove(GlobalSaveDataLoadDelegate);
	Json::Value root;
	Json::Reader reader;
	if (bSuccessfull)
	{
		const char *begining = (const char*)saveBlob.GetData();
		const char *end = begining + saveBlob.GetAllocatedSize();

		if (!reader.parse(begining, end, root))
		{
			return;
		}
	}
	else
	{
		if (localUserNum == 0)	// On consoles only the 1st user has the cloud service message
		{
			SetDisplayCloudServicesFeature(true);
		}
		//No save data exists so create it
		CreateNewState();
		Save(localUserNum);

		// #D11.CM
		OnGlobalSaveDataLoaded.Broadcast(this, localUserNum);
		return;
	}

	AfterLoadGlobalSaveData(root, localUserNum);

#endif
}

void UGlobalStateData::AfterLoadGlobalSaveData(Json::Value &root, int32 localUserNum)
{
	using namespace json::global;
	//////////////////////////////////////////////////////////////////////////
	// ACTUAL DATA:

	mVersion = root["version"].asInt(0);
	if (mVersion != SAVE_VERSION) {

	}

	mRecordedData.mAccountLinked = root["msa_account_linked"].asBool(0);
	mRecordedData.mInitialBootFlowComplete = root["Initial_boot_flow_complete"].asBool(0);
	mRecordedData.mXblActive = root["XblActive"].asBool(1);
	mRecordedData.mCrossplay = root["Crossplay"].asBool(1);
	mRecordedData.SelectedGameMode = root["selected_game_mode"].asUInt();
	mRecordedData.mLocale = stringutil::toFString(root["locale"].asString(""));
	
#if CLOUDSAVE_ENABLED
	mRecordedData.mCloudNag = root[CloudNagKey].asBool(1);
#else
	mRecordedData.mCloudNag = false;
#endif

	if (root.isMember("reconnect")) {
		mRecordedData.mReconnect = parseReconnect(root["reconnect"]);
	}

	if (root.isMember("recentFiles")) {
		mRecordedData.mRecentSaveFilenames = parseRecentFiles(root["recentFiles"]);
	}

	// D11.DB
	if (root.isMember("keybinds2")) {
		auto nodeKeybinds = root["keybinds2"];
		if (nodeKeybinds.isMember("player0")) {
			mRecordedData.mKeybinds = parseKeybinds(nodeKeybinds["player0"]);
		}
	}

	// D11.LG Start
	if (root.isMember("settings"))
	{
		mRecordedData.mSettingsNode = root["settings"];
	}

	// D11.SSN
	if (root.isMember(TrackedStatsKey)) {
		mRecordedData.mTrackedStats = parseTrackedStats(root[TrackedStatsKey], mVersion);
		mTrackStatsNode = root[TrackedStatsKey];
	}


	for (const auto& skinId : root["skinsSelected"]) {
		mRecordedData.mSkinsSelected.Add(stringutil::toFName(skinId.asString()));
	}

	if (root.isMember("seenNews")) {
		Json::Value &seenNews = root["seenNews"];

		mRecordedData.mSeenNews.Reset();

		//Only use the last news threshold values from read in save data to prevent instability
		const int iNumSeenNewsItems = seenNews.size();
		const int iNumSeenNewsStart = FMath::Max(iNumSeenNewsItems - sSeenNewsPruneThreshold, 0);
		
		for(int i = iNumSeenNewsStart; i < iNumSeenNewsItems; ++i)
		{
			mRecordedData.mSeenNews.Add(stringutil::toFString(seenNews[i].asString()));
		}

	}

	// D11.SSN
	if (root.isMember("title") && root["title"].asUInt() > 0) {
		mRecordedData.title = static_cast<ETitle>(root["title"].asUInt());
	}

	if (online::IsUsingCachedEntitlements() && root.isMember("cachedEntitlements")) {
		mRecordedData.mCachedEntitlements = parseCachedEntitlements(root["cachedEntitlements"]);
	}

	LoadAllCharacterProfiles(localUserNum);

	SetDisplayCloudServicesFeature(mRecordedData.mCloudNag);
}




void UGlobalStateData::RecordedData::RecordGlobalSaveDataToJsonNode(Json::Value& node) const
{
	using namespace json::global;
	using namespace json::common;

	node["version"] = SAVE_VERSION;
	node["locale"] = stringutil::toStdString(mLocale);
	node["reconnect"] = toJson(mReconnect);
	node["recentFiles"] = toJson(mRecentSaveFilenames);


	node[TrackedStatsKey] = toJson(mTrackedStats); // D11.SSN
	node["skinsSelected"] = iterableToJsonArray(mSkinsSelected);

	//Prune before save for safety
	if (mSeenNews.Num() > sSeenNewsPruneThreshold)
	{
		TArray<FString> mSafetySeenNews;

		const int iNumSeenNewsItems = mSeenNews.Num();
		const int iNumSeenNewsStart = iNumSeenNewsItems - sSeenNewsPruneThreshold;
		mSafetySeenNews.Reserve(sSeenNewsPruneThreshold);

		for (int i = iNumSeenNewsStart; i < iNumSeenNewsItems; ++i)
		{
			mSafetySeenNews.Add(mSeenNews[i]);
		}
		
		node["seenNews"] = iterableToJsonArray(mSafetySeenNews);
	}
	else
	{
		node["seenNews"] = iterableToJsonArray(mSeenNews);
	}

	

	node["msa_account_linked"] = mAccountLinked;
	node["Initial_boot_flow_complete"] = mInitialBootFlowComplete;

	node[CloudNagKey] = false;

	node["keybinds2"]["player0"] = toJson(mKeybinds); // D11.DB

	node["XblActive"] = mXblActive;
	node["Crossplay"] = mCrossplay;
	node["selected_game_mode"] = SelectedGameMode;

	// D11.LG
	node["settings"] = mSettingsNode;

	// D11.SSN
	node["title"] = Json::Value(static_cast<uint8>(title));

	if (online::IsUsingCachedEntitlements()) {
		node["cachedEntitlements"] = toJson(mCachedEntitlements);
	}
}

void UGlobalStateData::Save(int32 localUserNum) const {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UGlobalStateData_Save)


	//D11.KS - The first local player should be the only one to save the global save data on PC.
#if PLATFORM_WINDOWS || PLATFORM_SWITCH
	if(localUserNum > 0)
	{
		return;
	}
#endif



	//DG: only save when locally controlled ?
#if CONSOLE_SAVE_SYSTEM

	//D11.SC Moved the Json processing off the game thread as it was costing a few MS
	(new FAutoDeleteAsyncTask<FAsyncGlobalStateDataJsonSave>(mRecordedData, localUserNum))->StartBackgroundTask(DungeonsConsoleSave::Instance()->ConsoleSaveDataThreadPool.GetThreadPool());

#else
	Json::Value node;
	mRecordedData.RecordGlobalSaveDataToJsonNode(node);

	const FString savePath = GetGlobalSavePath(localUserNum) / "savefile" + globalsavedata::BinaryFileExtension;
	binarysavedata::safer::writeJsonToBinaryFileSafer(node, savePath);



#endif
}
#undef LOCTEXT_NAMESPACE



bool UGlobalStateData::DeleteByIndex(int32 localUserNum, int32 inIndex) {
	if (mSlots.IsValidIndex(inIndex)) {
		UCharacterSaveData* characterSaveData = mSlots[inIndex];
		characterSaveData->markedForDelete = true;

#if CONSOLE_SAVE_SYSTEM
		//D11.PS - Just delete the save data on console
		FString saveFilename = characterSaveData->GetBaseFilename();

		// #D11.CM - Removed character slot.
		if (APlayerCharacterSaveSlot* saveSlot = mCharacterSlots.FindAndRemoveChecked(characterSaveData->GetBaseFilename()).Get())
		{
			saveSlot->Destroy();
		}

		DungeonsConsoleSave::Instance()->DeleteAsync(characterSaveData->GetBaseFilename(), localUserNum);
		mSlots.Remove(characterSaveData);
		return true;
#else
		if (characterSaveData->MoveFileTo(GetDeletedCharacterSavePath(localUserNum) / characterSaveData->GetBaseFilename() + globalsavedata::BinaryFileExtension)) {
			mSlots.Remove(characterSaveData);
			return true;
		}
#endif
	}

	return false;
}

TArray<UCharacterSaveData*> UGlobalStateData::GetAllProfiles() {
	return mSlots;
}

void UGlobalStateData::LoadCharacterSaveData(FString path, int32 localUserNum)
{
	UCharacterSaveData* characterSaveData = NewObject<UCharacterSaveData>(this);
	characterSaveData->OnLoadFinished.AddDynamic(this, &UGlobalStateData::OnCharacterLoadFinished);
	characterSaveData->Load(path, localUserNum);
}

void UGlobalStateData::OnCharacterLoadFinished(bool bSuccessfull, int32 LocalUserNum, UCharacterSaveData* saveData)
{
	saveData->OnLoadFinished.RemoveDynamic(this, &UGlobalStateData::OnCharacterLoadFinished);

	if (bSuccessfull)
	{
		mSlots.Add(saveData);
	}

	CharacterSlotsLeftToCheck--;
	if (CharacterSlotsLeftToCheck == 0)
	{
		OnGlobalSaveDataLoaded.Broadcast(this, LocalUserNum);
	}
}

const ReconnectSaveData& UGlobalStateData::ReadReconnectState() const {
	return mRecordedData.mReconnect;
}

ReconnectSaveData& UGlobalStateData::GetReconnectState() {
	return mRecordedData.mReconnect;
}

// D11.DB
TArray<FKeybinding>& UGlobalStateData::GetKeybinds() {
	return mRecordedData.mKeybinds;
}

// D11.LG Start
Json::Value& UGlobalStateData::GetSettingsNode()
{
	return mRecordedData.mSettingsNode;
}
// D11.LG End

// D11.SSN
TrackedStats& UGlobalStateData::GetTrackedStats() {
	return mRecordedData.mTrackedStats;
}

Json::Value& UGlobalStateData::GetTrackedStatsNode() {
	return mTrackStatsNode;
}

bool UGlobalStateData::HasSelectedSkinId(const FName& skinId) const {
	return mRecordedData.mSkinsSelected.Contains(skinId);
}

void UGlobalStateData::AddSelectedSkinId(const FName& skinId) {
	mRecordedData.mSkinsSelected.Add(skinId);
}


bool UGlobalStateData::HasSeenNewsId(const FString& id) const {
	return mRecordedData.mSeenNews.Contains(id);
}

void UGlobalStateData::AddSeenNewsId(const FString& id) {
	if (mRecordedData.mSeenNews.Num() >= sSeenNewsPruneThreshold) {
		const int32 iNumToRemove = mRecordedData.mSeenNews.Num() - sSeenNewsPruneThreshold + 1;		
		mRecordedData.mSeenNews.RemoveAt(0, iNumToRemove);
	}
	mRecordedData.mSeenNews.Add(id);
}

int32 UGlobalStateData::GetIndexForFilename(const FString& filename) const
{
	for (int32 index = 0; index < mSlots.Num(); ++index) {
		//D11.PC Changed == to Contains because on PS4 the save name max num of characters is 32 and
		// our filename is bigger than that
#if CONSOLE_SAVE_SYSTEM
		if (filename.Contains(mSlots[index]->GetBaseFilename()))
		{
#else
		if (filename == mSlots[index]->GetBaseFilename())
		{
#endif
			return index;
		}
	}

	return 0;
}

APlayerCharacterSaveSlot* UGlobalStateData::GetCharacterSlotFor(UObject* WorldContextObject, UCharacterSaveData* saveData, bool forceRefreshSlot) {
	if (!saveData) {
		return nullptr;
	}

	if (auto character = mCharacterSlots.Find(saveData->GetBaseFilename())) {
		if (character->IsValid()) {
			if(forceRefreshSlot) {
				character->Get()->AssignSaveData(saveData);
			}
			return character->Get();
		}
	}

	// Spawn new Character Save Slot Actor
	if (APlayerCharacterSaveSlot* spawnedSlot = WorldContextObject->GetWorld()->SpawnActor<APlayerCharacterSaveSlot>()) {
		spawnedSlot->AssignSaveData(saveData);
		mCharacterSlots.Add(saveData->GetBaseFilename(), spawnedSlot);

		return spawnedSlot;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to spawn Character Slot Actor"));
	return nullptr;
}

void UGlobalStateData::SetPIESaveFolder(std::string PIEFolder) {
	mPIESaveFolder = PIEFolder;
}

ETitle UGlobalStateData::GetTitle() {
	return mRecordedData.title;
}

void UGlobalStateData::ChangeTitle(ETitle title) {
	mRecordedData.title = title;
}
