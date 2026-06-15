#pragma once

#include "AnalyticsDataTypes.h"
#include "AnalyticsReflection.h"
#include "game/LevelSettings.h"
#include <string>
#include <chrono>
#include "game/actor/character/player/BasePlayerController.h"


class ItemType;
class AActor;
class APlayerCharacter;
class AItemInstance;
class UItemSlot;
class PlayfabServicesEvent;

namespace game { class Game; }
namespace game { class Tile; using TileRef = const Tile&; }

namespace analytics {

class Analytics {
protected:
	explicit Analytics();
public:
	struct CrashData {
		bool ShutdownSuccess = false;
		std::string DateTime;
		std::string AppSessionId;
		long AppSessionUptimeMs = 0;
		int BuildNumber = 0;
		std::string BuildFlavour;
		std::string BuildBranch;
		std::string BuildVersion;
		int LocalPlayerCount = 0;
		int TotalPlayerCount = 0;
		std::string UIScene;
		int LevelId = 0;
		std::string TileId;
		int TileOrientation = 0;

		void writeString(std::ofstream& stream, std::string& str) {
			size_t size = DateTime.length();
			stream.write(reinterpret_cast<char*>(&size), sizeof(size_t));
			stream.write(str.c_str(), size);
		}

		void write(std::string fileName) {
			std::ofstream out(fileName, std::ios::binary | std::ios::trunc);
			if (out.is_open()) {
				out.write(reinterpret_cast<char*>(&ShutdownSuccess), sizeof(ShutdownSuccess));
				writeString(out, DateTime);
				writeString(out, AppSessionId);
				out.write(reinterpret_cast<char*>(&AppSessionUptimeMs), sizeof(AppSessionUptimeMs));
				out.write(reinterpret_cast<char*>(&BuildNumber), sizeof(BuildNumber));
				writeString(out, BuildFlavour);
				writeString(out, BuildBranch);
				writeString(out, BuildVersion);
				out.write(reinterpret_cast<char*>(&LocalPlayerCount), sizeof(LocalPlayerCount));
				out.write(reinterpret_cast<char*>(&TotalPlayerCount), sizeof(TotalPlayerCount));
				writeString(out, UIScene);
				out.write(reinterpret_cast<char*>(&LevelId), sizeof(LevelId));
				writeString(out, TileId);
				out.write(reinterpret_cast<char*>(&TileOrientation), sizeof(TileOrientation));
			}
			out.close();
		}

		void readString(std::ifstream& stream, std::string& str) {
			size_t size;
			char* data;

			stream.read(reinterpret_cast<char*>(&size), sizeof(size));
			data = new char[size + 1];
			stream.read(data, size);
			data[size] = '\0';
			str = data;
			delete data;
		}

		void read(std::string fileName) {
			std::ifstream in(fileName, std::ios::binary);
			if (in.is_open()) {
				in.read(reinterpret_cast<char*>(&ShutdownSuccess), sizeof(ShutdownSuccess));
				readString(in, DateTime);
				readString(in, AppSessionId);
				in.read(reinterpret_cast<char*>(&AppSessionUptimeMs), sizeof(AppSessionUptimeMs));
				in.read(reinterpret_cast<char*>(&BuildNumber), sizeof(BuildNumber));
				readString(in, BuildFlavour);
				readString(in, BuildBranch);
				readString(in, BuildVersion);
				in.read(reinterpret_cast<char*>(&LocalPlayerCount), sizeof(LocalPlayerCount));
				in.read(reinterpret_cast<char*>(&TotalPlayerCount), sizeof(TotalPlayerCount));
				readString(in, UIScene);
				in.read(reinterpret_cast<char*>(&LevelId), sizeof(LevelId));
				readString(in, TileId);
				in.read(reinterpret_cast<char*>(&TileOrientation), sizeof(TileOrientation));
			}
			in.close();
		}
	};

	static Analytics& GetInstance();

	Analytics(Analytics const&) = delete;
	void operator=(Analytics const&) = delete;

	void FireEmptyEvent(const FString& eventName);
	void FireEventRoundAwards(const analytics::AwardData& awardsData);
	void FireEventRateMission(int grade);
	void FireEventAssert(const FString& fileName, const FString& assertMessage);
	void FireEventPlayerLogin(const FString& userId, const FString& playerNickname, int localPlayerIndex);
	void FireEventPlayerLogout(const FString& userId, const FString& playerNickname);

	// Test document implementations
	void FireAppLaunch();
	void FireReportHardware();
	void FireReportCrash(const CrashData&);
	void FireReportShutdown();
	void FireLevelStarted(const game::Game*, const FLevelSettings&);
	void FireLevelEnded(EEndPlayReason::Type reason);
	void FireLevelPerformanceSummary(const game::Game*, const FLevelSettings&, const std::array<uint32, FRAME_COUNT_ARRAY_SIZE>& frames, const std::array<uint32, DELTA_ARRAY_SIZE>& frameDeltas);
	void FireItemAcquired(const APlayerCharacter&, const FInventoryItemData&);
	void FireItemUsed(const AActor*, const AItemInstance&);
	void FirePlayerKilled(const APlayerCharacter&, AActor* killer, AActor* with);
	void FireNPCKilled(const AMobCharacter&, AActor* killer, AActor* with);
	void FireWeaponUsed(const APlayerCharacter&, const UItemSlot&, AActor* attacked, float damage, float preHealth, float postHealth);
	void FirePerformanceMetrics(const game::Game*, const APlayerCharacter&, const PerformanceTelemetry&);
	void FireHeartBeat(bool, const TArray<FString>& localUserIds, const TArray<FString>& networkUserIds, const TArray<FString>& networkPlatforms);
	void FireError(FString errorScene, int errorCode, FString errorString);
	void FireSettingsChanged();
	void FirePlayerLeveledUp(const APlayerCharacter* player, int32 level);
	void FireEventMerchantPurchase(const APlayerCharacter& player, const AMerchantBase& merchant, const FInventoryItemData& purchasedItem, TOptional<FMerchantPricing> price);
	void FireEventSkinSelected(const FString& slot, const FString& skinId);
	void FireEventItemsSacrificed(const APlayerCharacter* player, const FMissionOfferings& offerings);

	void FireIntegrityCheckFailed();	
	void FireIntegrityCheckDecoy();	

	void FireReportConnectionType(bool IsUsingRelay);

	const FString AppSessionId() const { return mAppSessionId; }

	struct CachedData {
		const game::Game* game = nullptr;
		TOptional<FString> roundId;
		TOptional<FLevelSettings> levelSettings;
		long mAppStartTimeMs;
	};
private:
	struct SessionData {
		TOptional<FString> SessionId;
		TArray<TPair<FString, FString>> PlayerIds;
	};
	SessionData GetCachedSessionData() const;
	SessionData GetSessionData() const;
	void FireHeroUpdated(const APlayerCharacter* player, FString reason);

	inline void FireIntegrityCheckEvent(int location);
	inline int GetIntegrityCheckLocationValue(bool failed) const;
	
	void WriteCrashData(const APlayerCharacter& player, bool shutdown);

	struct EventBuilder Create(const FString& eventName);
	struct EventBuilder Create(const FString& eventName, const SessionData&);
	struct EventBuilder _CreateKillEvent(const FString& eventName, const AActor& killed, const AActor* killer, const AActor* with);

	// Setters for storing data used in test document fields
	void SetGame(const game::Game*);
	void SetLevelSettings(const FLevelSettings&);

	// Test document implementations
	void _Base(PlayfabServicesEvent&, const SessionData&);

	CachedData mCachedData;
	int mRunningEventSequenceId;
	FString mAppSessionId;
	TOptional<FString> mUserId;
	FString mCommitHash;
	FString mBuildNumber;
	FString mBuildBranch;
	bool mLevelInProgress{ false };
	int mNumLocalPlayers{ 0 };
	int mNumRemotePlayers{ 0 };
	FString mDeviceId;

	mutable SessionData mCachedSessionData;
	mutable double mCachedSessionDataLastUpdatedSeconds = -1000.;
};

}