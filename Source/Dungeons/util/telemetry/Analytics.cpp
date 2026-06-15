
#include "Dungeons.h"
#include "Analytics.h"

#include "game/Game.h"
#include "game/LevelSettings.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/item/Arrow.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ItemSlot.h"
#include "game/item/ItemType.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/item/instance/GearItemInstance.h"
#include "game/level/GameTile.h"
#include "world/entity/MobTags.h"
#include "util/Algo.hpp"
#include "util/DungeonsAwardGenerator.h"
#include "PlayfabServices.h"
#include <PlatformMisc.h>
#include <GenericPlatformDriver.h>
#include "util/StringUtil.h"
#include "game/component/WalletComponent.h"
#include "game/Conversion.h"
#include "online/sessions/OnlineUtil.h"
#include "online/OnlineCommon.h"
#include "platform/GameVersion.h"
#include "SharedConstants.h"
#include "online/crossplay/Identity.h"


namespace analytics {

namespace empty {
	const std::string string     = "NotSet";
	const FString     fstring    = "NotSet";
	const FString     guidString = FGuid().ToString();
	const FVector     fvector    = FVector(-1);
	constexpr int     integer    = -1;
	const TArray<FEnchantmentData> enchantments;
}

namespace internal {
	
void FireAsyncEvent(AsyncTaskCallback callback) {
	(new FAutoDeleteAsyncTask<FCallbackAsyncTask>(callback))->StartBackgroundTask();
}

}

FString valueOr(FString s, FString orValue = empty::fstring) { return s.IsEmpty() ? orValue : s; }
FString valueOr(const TOptional<FString>& s, FString orValue = empty::fstring) { return s.Get(orValue); }

int valueOr(const TArray<FEnchantmentData>& enchantments, int index) {
	return (enchantments.Num() > index && enchantments[index].TypeID != EEnchantmentTypeID::Unset) ? static_cast<int>(enchantments[index].TypeID) : -1;
}

constexpr int BytesPerMiB = 1024 * 1024;

struct EventBuilder {
	EventBuilder(PlayfabServicesEvent e, const Analytics::CachedData& cached)
		: e(std::move(e))
		, cached(cached) {
	}

	EventBuilder& LevelBase() {
		if (!cached.levelSettings) {
			return *this;
		}
		auto& settings = cached.levelSettings.GetValue();
		e.addProperty("RoundId", valueOr(cached.roundId));
		e.addProperty("LevelId", static_cast<int>(settings.getLevelName()));
		e.addProperty("LevelIdLabel", GetEnumValueToStringStripped(settings.getLevelName()));
		e.addProperty("DifficultyId", static_cast<int>(settings.getDifficulty())); // @note Only difficulty?
		e.addProperty("ThreatId", static_cast<int>(settings.getThreatLevel())); // @note @added threat. What about extrachallenge?
		return *this;
	}

	EventBuilder& NPCBase(const AActor* actor) {
		auto* npc = Cast<AMobCharacter>(actor);
		e.addProperty("NPCInstanceId", npc? npc->GetInstanceId_ServerOnly().ToString() : empty::guidString);
		e.addProperty("NPCId", npc? static_cast<int>(npc->EntityType) : -1);
		//e.addProperty("NPCTypeId") // @todo
		return *this;
	}

	EventBuilder& Position(const FString& key, FVector pos) { e.addVectorProperties(key, pos); return *this; }
	EventBuilder& Position(const FString& key, const AActor* actor) { return Position(key, actor ? actor->GetActorLocation() : empty::fvector); }
	EventBuilder& PositioningBase(FVector pos) { return Position("Position", pos); }
	EventBuilder& PositioningBase(const AActor* actor) { return Position("Position", actor); }

	EventBuilder& TileBase(const AActor* actor) { return TileBase(_getTile(actor)); }
	EventBuilder& TileBase(BlockPos pos) { return TileBase(_getTile(pos)); }
	EventBuilder& TileBase(game::TilePtr tile) {
		e.addProperty("TileId", tile ? stringutil::toFString(tile->meta().id) : empty::fstring);
		e.addProperty("TileOrientation", tile ? quadrantToDegrees(tile->tilePlacement().originalPlacement()->placement().rotation) : -1);
		return *this;
	}

	EventBuilder& ItemTypeBase(const AItemInstance* instance) { return ItemTypeBase(instance? &instance->GetItemType() : nullptr); }
	EventBuilder& ItemTypeBase(const ItemType* item) { return _ItemTypeBase("Item", item); }
	EventBuilder& WeaponBase(const AActor* weaponActor) {
		const auto* weapon = Cast<AGearItemInstance>(weaponActor);
		return _ItemTypeBase("Weapon", weapon? &weapon->GetItemType() : nullptr);
	}
	EventBuilder& _ItemTypeBase(const FString& prefix, const ItemType* item) {
		e.addProperty(prefix + "Id", item? item->getId().GetBackingType().ToString() : "" );
		e.addProperty(prefix + "TypeId", item? static_cast<int>(item->getTag()) : -1 );
		return *this;
	}

	EventBuilder& ItemBase(const FInventoryItemData& itemData) {
		const auto& enchantments = itemData.Enchantments;
		e.addProperty("EnchantmentId_1_1", valueOr(enchantments, 0));
		e.addProperty("EnchantmentId_1_2", valueOr(enchantments, 1));
		e.addProperty("EnchantmentId_1_3", valueOr(enchantments, 2));
		e.addProperty("EnchantmentId_2_1", valueOr(enchantments, 3));
		e.addProperty("EnchantmentId_2_2", valueOr(enchantments, 4));
		e.addProperty("EnchantmentId_2_3", valueOr(enchantments, 5));
		e.addProperty("EnchantmentId_3_1", valueOr(enchantments, 6));
		e.addProperty("EnchantmentId_3_2", valueOr(enchantments, 7));
		e.addProperty("EnchantmentId_3_3", valueOr(enchantments, 8));
		return *this;
	}

	EventBuilder& ItemBase(const AActor* instanceActor) {
		if (auto* instance = Cast<AItemInstance>(instanceActor)) {
			ItemBase(instance->OptionalItemDataSource.Get({}));
		}
		return *this;
	}

	EventBuilder& EnchantmentApplicationBase(const AActor* instanceActor) {
		if (auto* instance = Cast<AItemInstance>(instanceActor)) {
			const auto applied = algo::copy_if(instance->OptionalItemDataSource.Get({}).Enchantments, RETLAMBDA(it.Level));
			e.addProperty("EnchantmentIdApplied1", valueOr(applied, 0));
			e.addProperty("EnchantmentIdApplied2", valueOr(applied, 1));
			e.addProperty("EnchantmentIdApplied3", valueOr(applied, 2));
		}
		return *this;
	}

	EventBuilder& CpuGpuModel() {
		e.addProperty("CoreCount", FPlatformMisc::NumberOfCores()); // @changed from: CPUCount
		e.addProperty("CPU0Model", valueOr(FPlatformMisc::GetCPUBrand()));
		e.addProperty("GPUCount", GNumExplicitGPUsForRendering);
		e.addProperty("GPU0Model", valueOr(FPlatformMisc::GetPrimaryGPUBrand()));
		e.addProperty("GPU0DriverVersion", valueOr(GRHIAdapterUserDriverVersion));
		return *this;
	}

	EventBuilder& QualitySettings() {
		//e.addProperty("QualitySetting", "");
		//e.addProperty("IsQualitySettingPreset", false);
		return *this;
	}

	EventBuilder& PerformanceBase() {
		CpuGpuModel();
		QualitySettings();
		return *this;
	}

	EventBuilder& Aggregate() {
		e.setShouldAggregate(true); return *this;
	}

	PlayfabServicesEvent& evt() {
		return e;
	}

	operator PlayfabServicesEvent() & { return e; }
	operator PlayfabServicesEvent() && { return std::move(e); }
private:
	game::TilePtr _getTile(BlockPos pos) const {
		return cached.game ? cached.game->tiles().getTile(pos) : nullptr;
	}
	game::TilePtr _getTile(const AActor* actor) const {
		return _getTile(*actor);
	}

	PlayfabServicesEvent e;
	const Analytics::CachedData& cached;
};

Analytics& Analytics::GetInstance() {
	static Analytics s_analytics;
	return s_analytics;
}

Analytics::Analytics()
	: mRunningEventSequenceId(0)
	, mAppSessionId(FGuid::NewGuid().ToString())
	, mDeviceId(FPlatformMisc::GetDeviceId())
	//, mAppSessionId(FApp::GetSessionId().ToString(EGuidFormats::DigitsWithHyphens)) // @todo: we want this instead, but it crashed
{
	mCachedData.mAppStartTimeMs = static_cast<long>(FApp::GetCurrentTime() * 1000.0);

	auto buildVersion = UGameVersion::BuildVersion();
	TArray<FString> tokens;
	buildVersion.ParseIntoArray(tokens, TEXT(":"), true);
	if (tokens.Num() == 3) {
		mBuildNumber = tokens[0];
		mBuildBranch = tokens[1];
		mCommitHash = tokens[2];
	} else {
		mCommitHash = FString("UNKNOWN_HASH");
		mBuildNumber = FString("");
		mBuildBranch = FString("LOCALBUILD");
	}

	/*CrashData crashData;
	auto folder = stringutil::toStdString(FPaths::ProjectSavedDir());
	crashData.read(folder + "analytics_crashdata.dat");
	if (!crashData.ShutdownSuccess) {
		FireReportCrash(crashData);
	}*/
}

void Analytics::FireEmptyEvent(const FString& eventName) {

	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireEmptyEvent_);
    PlayfabServices::SendTelemetry(Create(eventName));
}

void Analytics::FireEventRoundAwards(const analytics::AwardData& awardsData) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireEventRoundAwards_);
	PlayfabServicesEvent e = Create("RoundAwards");
    
    e.addProperty("Score", awardsData.score);
    e.addProperty("PlayerId", awardsData.playerNumber);
    e.addProperty("AwardType", GetEnumValueToStringStripped(awardsData.type));

    PlayfabServices::SendTelemetry(e);
}

void Analytics::FireEventRateMission(int grade) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireEventRateMission_);
	PlayfabServicesEvent e = Create("RateMission");

    e.addProperty("Grade", grade);

    PlayfabServices::SendTelemetry(e);
}

void Analytics::FireEventAssert(const FString& fileName, const FString& assertMessage) {
	PlayfabServicesEvent e = Create("Assert");

    e.addProperty("FileName", fileName);
    e.addProperty("Message", assertMessage);

    PlayfabServices::SendTelemetry(e);
}

void Analytics::FirePlayerKilled(const APlayerCharacter& player, AActor* killer, AActor* with) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FirePlayerKilled_);
	auto builder = Create("PlayerKilled")
		.LevelBase()
		.PositioningBase(&player)
		.Position("KillerPosition", killer)
		.TileBase(&player)
		.NPCBase(killer)
		.WeaponBase(with)
		.ItemBase(with)
		.EnchantmentApplicationBase(with);

	auto& e = builder.evt();
	e.addProperty("PlayerInstanceId", player.GetInstanceId_ServerOnly().ToString());
	PlayfabServices::SendTelemetry(e);
}

enum class EKillType { Unknown = -1, Suicide, Player, Pet, Npc };

EKillType getKillType(const AActor& killed, const AActor* killer) {
	if (killer == &killed) { return EKillType::Suicide; }
	if (killer && killer->IsA<AMobCharacter>()) { return EKillType::Npc; }
	if (killer && killer->IsA<APlayerCharacter>()) { return EKillType::Player; }
	return EKillType::Unknown;
}

void Analytics::FireNPCKilled(const AMobCharacter& mob, AActor* killer, AActor* with) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireNPCKilled_);

	TWeakObjectPtr<AActor> WeakWithObject = with;
	TWeakObjectPtr<AActor> WeakKillerObject = killer;
	int iKillType = static_cast<int>(getKillType(mob, killer));
	uint32 iEntityType = static_cast<uint32>(mob.EntityType);
	bool isMiniBoss = mob.HasTag(MobTags::HashTag_Miniboss);
	bool isAncient = mob.HasTag(MobTags::HashTag_Ancient);

	internal::FireAsyncEvent([this, WeakWithObject, WeakKillerObject, iKillType, iEntityType, sessionData = GetCachedSessionData(), isMiniBoss, isAncient]()
		{
			if (WeakWithObject.IsValid() && WeakKillerObject.IsValid())
			{
				AActor* weaponUsed = WeakWithObject.Get();
				bool isMappedToBow = false;
				if (weaponUsed->IsA<ABaseProjectile>() && WeakKillerObject->IsA<APlayerCharacter>())
				{
					//Arrow shot by player
					const auto player = Cast<APlayerCharacter>(WeakKillerObject.Get());
					const auto equipment = player != nullptr ? player->GetEquipmentComponent() : nullptr;
					const auto rangedSlot = equipment != nullptr ? equipment->GetSlotsOfType(ESlotType::RangedWeapon) : TArray<UItemSlot*>();
					if (rangedSlot.Num() > 0 && rangedSlot[0] != nullptr)
					{
						weaponUsed = rangedSlot[0]->GetItem();
						isMappedToBow = true;
					}
				}

				EventBuilder builder = Create("NPCKilled", sessionData)
					.LevelBase()
					.WeaponBase(weaponUsed)
					.ItemBase(weaponUsed)
					.EnchantmentApplicationBase(weaponUsed)
					.Aggregate();

				auto& e = builder.evt();
				e.setAggregationTime(180);
				e.addProperty("KillTypeId", iKillType);
				e.addProperty("EntityType", iEntityType);
				e.addProperty("IsMiniBoss", isMiniBoss ? 1 : 0);
				e.addProperty("IsAncient", isAncient ? 1 : 0);
				if (isMappedToBow)
				{
					e.addProperty("ProjectileName", WeakWithObject->GetName());
				}
				e.addMeasurement("Count", PlayfabServicesMeasurement::AggregationType::Sum, 1);


				PlayfabServices::SendTelemetry(e);
				}

		});
}

void Analytics::FireWeaponUsed(const APlayerCharacter& player, const UItemSlot& slot, AActor* attacked, float damage, float preHealth, float postHealth) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireWeaponUsed_);
	
	TWeakObjectPtr<AItemInstance> WeakObject = slot.GetItem();

	internal::FireAsyncEvent([&, WeakObject, damage, sessionData = GetCachedSessionData()]()
	{
		if (WeakObject.IsValid())
		{
			PlayfabServicesEvent e = Create("WeaponUsed", sessionData)
				.LevelBase()
				.ItemTypeBase(WeakObject.Get())
				.EnchantmentApplicationBase(WeakObject.Get())
				.ItemBase(WeakObject.Get())
				.Aggregate();

			e.addMeasurement("Damage", PlayfabServicesMeasurement::AggregationType::Sum, damage);

			PlayfabServices::SendTelemetry(e);
		}
	});
}

void Analytics::FirePerformanceMetrics(const game::Game* game, const APlayerCharacter& player, const PerformanceTelemetry& perfTelemetry) {
	if (perfTelemetry.NumSamples == 0)
		return;

	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FirePerformanceMetrics_);

	PlayfabServicesEvent e = Create("PerformanceMetrics")
		.LevelBase()
		.PerformanceBase()
		.Aggregate();

	e.addProperty("IsGamePaused", game->world().IsPaused());
	e.addProperty("InSeamlessTravel", game->world().IsInSeamlessTravel());
	e.addProperty("IsLobbyLevel", game->IsLobbyLevel());
	e.addProperty("IsInInventory", player.IsInInventory());
	e.addProperty("IsRespawning", player.IsRespawning());

	e.setAggregationTime(10);
	e.addMeasurement("Samples", PlayfabServicesMeasurement::AggregationType::Increment, 1);
	e.addMeasurement("AvgPlayerFrameTimeMS", PlayfabServicesMeasurement::AggregationType::Average, perfTelemetry.PlayerFrameTimeMS / perfTelemetry.NumSamples);
	e.addMeasurement("AvgGPUFrameTimeMS", PlayfabServicesMeasurement::AggregationType::Average, perfTelemetry.GPUFrameTimeMS / perfTelemetry.NumSamples);
	e.addMeasurement("AvgGameThreadTimeMS", PlayfabServicesMeasurement::AggregationType::Average, perfTelemetry.GameThreadTimeMS / perfTelemetry.NumSamples);
	e.addMeasurement("AvgRenderThreadTimeMS", PlayfabServicesMeasurement::AggregationType::Average, perfTelemetry.RenderThreadTimeMS / perfTelemetry.NumSamples);

	const auto memData = FPlatformMemory::GetStats();
	e.addMeasurement("AvgUsedPhysicalMemoryMB", PlayfabServicesMeasurement::AggregationType::Average, static_cast<uint32_t>(memData.UsedPhysical / BytesPerMiB));
	e.addMeasurement("AvgUsedVirtualMemoryMB", PlayfabServicesMeasurement::AggregationType::Average, static_cast<uint32_t>(memData.UsedVirtual / BytesPerMiB));
	e.addMeasurement("AvgPeakUsedPhysicalMemoryMB", PlayfabServicesMeasurement::AggregationType::Average, static_cast<uint32_t>(memData.PeakUsedPhysical / BytesPerMiB));
	e.addMeasurement("AvgPeakUsedVirtualMemoryMB", PlayfabServicesMeasurement::AggregationType::Average, static_cast<uint32_t>(memData.PeakUsedVirtual / BytesPerMiB));
	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireHeartBeat(bool playerActive, const TArray<FString>& localUserIds, const TArray<FString>& networkUserIds, const TArray<FString>& networkPlatforms) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireHeartBeat_);
	mNumRemotePlayers = networkUserIds.Num();
	mNumLocalPlayers = localUserIds.Num();

	FString localUserIdsString = FString::Join(localUserIds, TEXT(","));
	FString NetworkUserIdsString = FString::Join(networkUserIds, TEXT(","));
	FString NetworkPlatformsString = FString::Join(networkPlatforms, TEXT(","));

	internal::FireAsyncEvent([&, playerActive, localUserIdsString, NetworkUserIdsString, NetworkPlatformsString, sessionData = GetCachedSessionData()]()
	{
		PlayfabServicesEvent e = Create("HeartBeat", sessionData)
			.LevelBase();

		e.addProperty("HasInput", playerActive);
		e.addProperty("LocalUserIds", localUserIdsString);
		e.addProperty("NetworkUserIds", NetworkUserIdsString);
		e.addProperty("NetworkPlatforms", NetworkPlatformsString);

		PlayfabServices::SendTelemetry(e);
	});
}

void Analytics::FireError(FString errorScene, int errorCode, FString errorString) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireError_);
	PlayfabServicesEvent e = Create("Error");

	e.addProperty("ErrorScene", errorScene);
	e.addProperty("ErrorCode", errorCode);
	e.addProperty("ErrorString", errorString);

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireSettingsChanged() {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireSettingsChanged_);
	PlayfabServicesEvent e = Create("SettingsChanged")
		.QualitySettings();

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireIntegrityCheckFailed() {
	FireIntegrityCheckEvent(GetIntegrityCheckLocationValue(true));
}

void Analytics::FireIntegrityCheckDecoy() {
	FireIntegrityCheckEvent(GetIntegrityCheckLocationValue(false));
}

void Analytics::FireReportConnectionType(bool IsUsingRelay)
{
	PlayfabServicesEvent e = Create("PlayerConnected");
	e.addProperty(FString("UsingRelays"), IsUsingRelay);

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireIntegrityCheckEvent(const int location) {
	PlayfabServicesEvent e = Create("AppHandled");

	e.addProperty("Location", location);

	PlayfabServices::SendTelemetry(e);
}

int Analytics::GetIntegrityCheckLocationValue(const bool failed) const {
	const auto controlBit = 3;

	auto location = FMath::RandRange(10000, 99999);
	if (failed) {
		// Always set the control bit when failed
		location |= 1UL << controlBit;
	} else {
		// Always clear the control bit when not failed
		location &= ~(1UL << controlBit);
	}

	return location;
}

void Analytics::FireEventPlayerLogin(const FString& userId, const FString& playerNickname, int localPlayerIndex) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireEventPlayerLogin_);
	if (!mUserId) {
		mUserId = userId;
	}
	PlayfabServicesEvent e = Create("PlayerLogin"); // @note: these aren't in the QA Telemetry spec,
	PlayfabServices::SendTelemetry(e);             //        so I commented them out for now.
}

void Analytics::FireEventPlayerLogout(const FString& userId, const FString& playerNickname) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireEventPlayerLogout_);
	if (mUserId == userId) {
		mUserId.Reset();
	}
	PlayfabServicesEvent e = Create("PlayerLogout"); // @note: these aren't in the QA Telemetry spec,
	PlayfabServices::SendTelemetry(e);              //        so I commented them out for now.
}

void Analytics::FireAppLaunch() {
	PlayfabServicesEvent e = Create("AppLaunch");
	e.addProperty("Store", Common::getStoreName());
	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireItemUsed(const AActor* user, const AItemInstance& instance) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireItemUsed_);
	PlayfabServicesEvent e = Create("ItemUsed")
		.LevelBase()
		.PositioningBase(user)
		.TileBase(user)
		.ItemTypeBase(&instance);

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireReportHardware() {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireReportHardware_);
	PlayfabServicesEvent e = Create("ReportHardware").CpuGpuModel();

	e.addProperty("OSVersion", valueOr(FPlatformMisc::GetOSVersion()));
	e.addProperty("PhysicalMemoryMBytes", static_cast<uint32_t>(FPlatformMemory::GetConstants().TotalPhysical / BytesPerMiB)); // @changed from PhysicalMemoryBytes

	FDisplayMetrics displays;
	FDisplayMetrics::RebuildDisplayMetrics(displays);
	e.addProperty("DisplayCount", displays.MonitorInfo.Num());
	for (int i = 0; i < std::min(3, displays.MonitorInfo.Num()); ++i) {
		const auto displayId = "Display" + FString::FromInt(i);
		e.addProperty(displayId + "Height", displays.MonitorInfo[i].NativeHeight);
		e.addProperty(displayId + "Width",  displays.MonitorInfo[i].NativeWidth);
	}

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireReportCrash(const CrashData& crashData) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireReportCrash_);
	PlayfabServicesEvent e = Create("CrashReport");

	e.addProperty("CrashDateTime", FString(crashData.DateTime.c_str()));
	e.addProperty("CrashAppSessionId", FString(crashData.AppSessionId.c_str()));
	e.addProperty("CrashAppSessionUptimeMs", static_cast<const int32_t>(crashData.AppSessionUptimeMs));
	e.addProperty("CrashBuildNumber", crashData.BuildNumber);
	e.addProperty("CrashBuildFlavour", FString(crashData.BuildFlavour.c_str()));
	e.addProperty("CrashBuildBranch", FString(crashData.BuildBranch.c_str()));
	e.addProperty("CrashBuildVersion", FString(crashData.BuildVersion.c_str()));
	e.addProperty("CrashReason", FString());
	e.addProperty("CrashLocalPlayerCount", crashData.LocalPlayerCount);
	e.addProperty("CrashTotalPlayerCount", crashData.TotalPlayerCount);
	e.addProperty("CrashUIScene", FString(crashData.UIScene.c_str()));
	e.addProperty("CrashLevelId", crashData.LevelId);
	e.addProperty("CrashTileId", crashData.LevelId);
	e.addProperty("CrashTileOrientation", crashData.TileOrientation);

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireReportShutdown() {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireReportShutdown_);
	PlayfabServicesEvent e = Create("ReportShutDown");
	e.addProperty("ShutdownDateTime", FDateTime::Now().ToIso8601());
	e.addProperty("ShutdownAppSessionId", mAppSessionId);
	e.addProperty("ShutdownAppSessionUptimeMs", static_cast<const int32_t>(FApp::GetCurrentTime() * 1000.0) - static_cast<const int32_t>(mCachedData.mAppStartTimeMs));

	PlayfabServices::SendTelemetry(e);

	/*if (auto game = mCachedData.game) {
		if (auto playerController = game->world().GetFirstPlayerController()) {
			if (auto player = Cast<APlayerCharacter>(playerController->GetPawn())) {
				WriteCrashData(*player, true);
			}
		}
	}*/
}

void Analytics::FireLevelStarted(const game::Game* game, const FLevelSettings& levelSettings) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireLevelStarted_);
	SetGame(game);
	SetLevelSettings(levelSettings);
	PlayfabServicesEvent e = Create("LevelStarted").LevelBase();
	PlayfabServices::SendTelemetry(e);
	mLevelInProgress = true;

	auto players = game->getPlayers();
	for (auto& p : players)	{
		if(p->IsLocallyControlled()){
			FireHeroUpdated(p.Get(), "LevelStarted");
		}
	}
}

void Analytics::FireLevelEnded(EEndPlayReason::Type reason) {
	//TOOD: Its very dangerous to keep a raw pointer to an object which is owned by a smart pointer. 
	//      Consider refactoring this class so that a pointer to game::Game is not stored here. 
	//      Or change mGame in GameBP to be a shared pointer. 
	//      For now, this event fires from AGameBP::EndPlay which happens before game::Game goes out
	//      of scope. This will prevent our pointer from pointing to freed memory. 
	PlayfabServicesEvent e = Create("LevelEnded").LevelBase();
	e.addProperty("Reason", reason);
	PlayfabServices::SendTelemetry(e);

	if (mCachedData.game)
	{
		auto players = mCachedData.game->getPlayers();
		for (auto& p : players) {
			if (p.IsValid() && p->IsLocallyControlled()) {
				FireHeroUpdated(p.Get(), "LevelEnded");
			}
		}
	}

	SetGame(nullptr);
	SetLevelSettings(FLevelSettings{});
	mLevelInProgress = false;
}

void Analytics::FireLevelPerformanceSummary(const game::Game* game, const FLevelSettings& levelSettings, const std::array<uint32, FRAME_COUNT_ARRAY_SIZE>& frames, const std::array<uint32, DELTA_ARRAY_SIZE>& frameDeltas) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireLevelPerformanceSummary_);
	// D11.DH
	// Fix for crash which occurs when accepting an invite whilst joining a session already on xbox one
	// Getting a null game pointer because the gamebp is in an invalid state
	if (!game) {
		return;
	}
	PlayfabServicesEvent e = Create("LevelPerformanceSummary")
		.LevelBase()
		.PerformanceBase();

	std::stringstream frameTimes;
	for (int i = 0; i < frames.size(); ++i)	{
		frameTimes << frames[i] << ",";
	}

	std::stringstream frameTimeDeltas;
	for (int i = 0; i < frameDeltas.size(); ++i) {
		frameTimeDeltas << frameDeltas[i] << ",";
	}

	e.addProperty("FrameTimeArray", FString(frameTimes.str().substr(0, frameTimes.str().length() - 1).c_str()));
	e.addProperty("FrameTimeDeltaArray", FString(frameTimeDeltas.str().substr(0, frameTimeDeltas.str().length() - 1).c_str()));

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireItemAcquired(const APlayerCharacter& player, const FInventoryItemData& itemData) {
	
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FireItemAcquired_);
	
	TWeakObjectPtr<APlayerCharacter> WeakPlayerObject = &player;
	FInventoryItemData newItemData = itemData;
	const auto emeraldBalance = player.GetOwnedEmeralds_Implementation();
	const auto goldBalance = player.GetOwnedGold_Implementation();
	FItemId itemId = itemData.GetItemId();

	internal::FireAsyncEvent([this, WeakPlayerObject, newItemData, emeraldBalance, goldBalance, itemId, sessionData = GetCachedSessionData()]()
	{
		if (WeakPlayerObject.IsValid())
		{
			PlayfabServicesEvent e = Create("ItemAcquired", sessionData)
				.LevelBase()
				.PositioningBase(WeakPlayerObject.Get())
				.TileBase(WeakPlayerObject.Get())
				.ItemBase(newItemData)
				._ItemTypeBase("Item", &GetItemRegistry().Get(itemId));


			e.addProperty("EmeraldBalance", emeraldBalance);
			e.addProperty("GoldBalance", goldBalance);

			PlayfabServices::SendTelemetry(e);
		}
	});
}

static void GetEnchantmentStrings(TArray<FEnchantmentData> enchants, FString& typeids, FString& levels) {
	typeids.Empty();
	levels.Empty();
	for(int i = 0; i < enchants.Num(); ++i)	{
		if (i != 0) {
			typeids += ",";
			levels += ",";
		}
		typeids += FString::FromInt(static_cast<uint8_t>(enchants[i].TypeID));
		levels += FString::FromInt(enchants[i].Level);
	}
}

Analytics::SessionData Analytics::GetCachedSessionData() const {
	check(IsInGameThread());
	if (FApp::GetCurrentTime() < mCachedSessionDataLastUpdatedSeconds + 0.1) {
		return mCachedSessionData;
	}
	return GetSessionData();
}

Analytics::SessionData Analytics::GetSessionData() const {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_GetSessionData_);
	check(IsInGameThread());
	if (mCachedData.game == nullptr) {
		return {};
	}
	SessionData sessionData;
	if (auto sessionInterface = online::getSessionInterface()) {
		const auto namedCrossplaySession = sessionInterface->GetNamedSession(DungeonsGameSessionName);
		if (namedCrossplaySession != nullptr && namedCrossplaySession->SessionInfo.IsValid()) {
			sessionData.SessionId = namedCrossplaySession->SessionInfo->GetSessionId().ToString();
		}
	}

	auto addPlayerId = [](TArray<TPair<FString, FString>>& playerIds, int index, const FString& id) {
		playerIds.Emplace("PlayerId" + FString::FromInt(index), id);
	};

	if (const auto& session = online::getCurrentSession()) {
		for (int i = 0; i < session->RegisteredPlayers.Num(); ++i) {
			addPlayerId(sessionData.PlayerIds, i, session->RegisteredPlayers[i]->ToString());
		}
	}
	else {
		for (int i = 0; i < PlatformMaxPlayers; ++i) {
			if (const auto& playerId = online::getIdentityInterface()->GetUniquePlayerId(i)) {
				addPlayerId(sessionData.PlayerIds, i, playerId->ToString());
			}
		}
	}
	mCachedSessionData = sessionData;
	mCachedSessionDataLastUpdatedSeconds = FApp::GetCurrentTime();
	return sessionData;
}

void Analytics::FireHeroUpdated(const APlayerCharacter* player, FString reason) {
	if (!player->IsLocallyControlled()) {
		return;
	}

	PlayfabServicesEvent e = Create("HeroUpdated")
		.LevelBase()
		.PositioningBase(player)
		.TileBase(player);

	e.addProperty("Reason", reason);
	auto onlineID = player->GetOnlineUserId();
	e.addProperty("HeroUpdatedId", onlineID.IsSet() ? onlineID.GetValue() : "");
	e.addProperty("HeroLevel", player->GetCharacterLevel());
	e.addProperty("HeroPower", player->GetTotalEquippedGearPower_Implementation());
	e.addProperty("EmeraldBalance", player->GetOwnedEmeralds_Implementation());
	e.addProperty("GoldBalance", player->GetOwnedGold_Implementation());

	auto stash = player->FindComponentByClass<UItemStashComponent>();
	if (stash != nullptr) {
		e.addProperty("UnspentPoints", stash->AvailableEnchantmentPoints());
	}

	if (player->GetEquipmentComponent() != nullptr) {
		FString types;
		FString levels;
		auto equippedMelee = player->GetEquipmentComponent()->GetSlot(ESlotType::MeleeWeapon).GetItem();
		if (equippedMelee != nullptr) {
			e.addProperty("MeleeId", equippedMelee->GetItemType().getId().GetBackingType().ToString());
			GetEnchantmentStrings(equippedMelee->OptionalItemDataSource.Get({}).Enchantments, types, levels);
			e.addProperty("MeleeEnchants", types);
			e.addProperty("MeleeEnchantLevels", levels);
		}

		auto equippedArmor = player->GetEquipmentComponent()->GetSlot(ESlotType::Armor).GetItem();
		if (equippedArmor != nullptr) {
			e.addProperty("ArmorId", equippedArmor->GetItemType().getId().GetBackingType().ToString());
			GetEnchantmentStrings(equippedArmor->OptionalItemDataSource.Get({}).Enchantments, types, levels);
			e.addProperty("ArmorEnchants", types);
			e.addProperty("ArmorEnchantLevels", levels);
		}

		auto equippedRanged = player->GetEquipmentComponent()->GetSlot(ESlotType::RangedWeapon).GetItem();
		if (equippedRanged != nullptr) {
			e.addProperty("RangedId", equippedRanged->GetItemType().getId().GetBackingType().ToString());
			GetEnchantmentStrings(equippedRanged->OptionalItemDataSource.Get({}).Enchantments, types, levels);
			e.addProperty("RangedEnchants", types);
			e.addProperty("RangedEnchantLevels", levels);
		}

		int artifactCounter = 0;
		for (UItemSlot* slot : player->GetEquipmentComponent()->GetSlotsOfType(ESlotType::ActivePermanent)) {
			auto item = slot->GetItem();
			if (item == nullptr) {
				continue;
			}
			e.addProperty("Artifact " + FString::FromInt(artifactCounter) + " Id", item->GetItemType().getId().GetBackingType().ToString());
			++artifactCounter;
		}
	}
	PlayfabServices::SendTelemetry(e);
}

void Analytics::FirePlayerLeveledUp(const APlayerCharacter* player, int32 level) {
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Analytics_FirePlayerLeveledUp_);
	FireHeroUpdated(player, "LevelUp");
}

void Analytics::FireEventMerchantPurchase(const APlayerCharacter& player, const AMerchantBase& merchant, const FInventoryItemData& purchasedItem, TOptional<FMerchantPricing> price) {
	PlayfabServicesEvent e = Create("MerchantPurchase")
		.LevelBase()
		.PositioningBase(&player)
		.TileBase(player)
		.ItemBase(purchasedItem)
		._ItemTypeBase("Item", &GetItemRegistry().Get(purchasedItem.GetItemId()));

	e.addProperty("MerchantName", merchant.GetDisplayName().ToString());
	e.addProperty("EmeraldBalance", player.GetOwnedEmeralds_Implementation());
	e.addProperty("GoldBalance", player.GetOwnedGold_Implementation());
	if (price.IsSet()) {
		e.addProperty("ItemCost", price.GetValue().Price);
		e.addProperty("RebateApplied", price.GetValue().RebateApplied);
	}

	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireEventItemsSacrificed(const APlayerCharacter* player, const FMissionOfferings& offerings)
{
	PlayfabServicesEvent e = Create("ItemsSacrificed")
		.LevelBase()
		.PositioningBase(player)
		.TileBase(player);

	e.addProperty("TotalPower", offerings.getOfferedDisplayItemPower());
	e.addProperty("TotalEnchantmentPoints", offerings.offeredEnchantmentPoints);
	for (int i = 0; i < offerings.offeredItems.Num(); ++i)
	{
		e.addProperty("Offer" + FString::FromInt(i), offerings.offeredItems[i]->GetItemData().GetItemType().getId().GetBackingType().ToString());
		FString types;
		FString levels;
		GetEnchantmentStrings(offerings.offeredItems[i]->GetItemData().Enchantments, types, levels);
		e.addProperty("Offer" + FString::FromInt(i) + "Enchants", types);
		e.addProperty("Offer" + FString::FromInt(i) + "EnchantLevels", levels);
	}
	PlayfabServices::SendTelemetry(e);
}

void Analytics::FireEventSkinSelected(const FString& slot, const FString& skinId)
{
	PlayfabServicesEvent e = Create("SkinSelected");
	e.addProperty("SaveSlot", slot);
	e.addProperty("SkinId", skinId);
	PlayfabServices::SendTelemetry(e);
}

void Analytics::SetGame(const game::Game* game) {
	mCachedData.game = game;
	mCachedData.roundId = game ? FString(game->roundId().c_str()) : TOptional<FString>{};
}

void Analytics::SetLevelSettings(const FLevelSettings& settings) {
	mCachedData.levelSettings = settings;
}

void Analytics::WriteCrashData(const APlayerCharacter& player, bool shutdown) {
	auto tile = mCachedData.game->tiles().getTile(conversion::ueToBlock(player.GetActorLocation()));

	CrashData crashData{
		shutdown,
		std::string(TCHAR_TO_UTF8(*FDateTime::Now().ToIso8601())),
		std::string(TCHAR_TO_UTF8(*mAppSessionId)),
		static_cast<long>(FApp::GetCurrentTime() * 1000.0) - mCachedData.mAppStartTimeMs,
		0,
		// Look at all this casting madness
		std::string(TCHAR_TO_UTF8(*FString(EBuildConfigurations::ToString(FApp::GetBuildConfiguration())))),
		std::string(TCHAR_TO_UTF8(*valueOr(FApp::GetBranchName()))),
		std::string(TCHAR_TO_UTF8(*valueOr(FApp::GetBuildVersion()))),
		mNumLocalPlayers,
		mNumLocalPlayers + mNumRemotePlayers,
		"",
		0,
		tile ? tile->meta().id : "",
		tile ? quadrantToDegrees(tile->tilePlacement().originalPlacement()->placement().rotation) : -1
	};
	std::string folder(TCHAR_TO_UTF8(*FPaths::ProjectSavedDir()));
	crashData.write(folder + "analytics_crashdata.dat");
}

EventBuilder Analytics::Create(const FString& eventName) {
	check(IsInGameThread() && "Creating an event without explicit session data should only be done on the main thread.");
	return Create(eventName, GetSessionData());
}

EventBuilder Analytics::Create(const FString& eventName, const SessionData& sessionData) {
	PlayfabServicesEvent e(mUserId.Get(""), eventName);
	if (mCachedData.roundId) {
		e.addProperty("RoundId", mCachedData.roundId.GetValue());
	}
	_Base(e, sessionData);
	return EventBuilder(std::move(e), mCachedData);
}


void Analytics::_Base(PlayfabServicesEvent& e, const SessionData& sessionData) {
	e.addProperty("DeviceId", valueOr(mDeviceId));
	e.addProperty("DeviceLocation", FPlatformMisc::GetDefaultLocale());
	e.addProperty("AppSessionId", mAppSessionId);
	e.addProperty("BuildFlavour", FString(EBuildConfigurations::ToString(FApp::GetBuildConfiguration())));
	e.addProperty("BuildBranch", mBuildBranch);
	e.addProperty("BuildCommitHash", mCommitHash);
	e.addProperty("BuildNumber", mBuildNumber);
	e.addProperty("GameVersion", FString(UGameVersion::GetVersionNumber()));
	e.addProperty("IsDevBuild", Common::getGameDevVersionString().length() > 0 ? "True" : "False");
	e.addProperty("LocalPlayerCount", mNumLocalPlayers);
	e.addProperty("TotalPlayerCount", mNumLocalPlayers + mNumRemotePlayers);
	e.addProperty("DeviceFamily", valueOr(FPlatformProperties::IniPlatformName()));
	e.addProperty("DeviceModel", valueOr(FPlatformMisc::GetDeviceMakeAndModel()));
	e.addProperty("LevelBeingPlayed", mLevelInProgress);

	if (auto game = mCachedData.game) {
		
		if (const auto& sessionId = sessionData.SessionId) {
			e.addProperty("MultiplayerCorrelationId", sessionId.GetValue());
		}
		
		for (const auto& entry : sessionData.PlayerIds) {
			e.addProperty(entry.Key, entry.Value);
		}
		
		if (game->world().GetNetMode() == ENetMode::NM_Client)	{
			e.addProperty("IsHost", false);
		} else {
			e.addProperty("IsHost", true);
		}
	}
}

}