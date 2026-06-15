#pragma once

#include "Optional.h"
#include "UnrealString.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "game/Enchantments/EnchantmentType.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/InventoryItemData.h"
#include "game/realms/Realms.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/drop/EyeOfEnderDefines.h"
#include "game/cosmetics/CosmeticType.h"
#include "ui/hints/UIHintType.h"
#include "save/DungeonsConsoleSave.h" // D11.SSN
#include "game/mobspawn/EntityTypeMappers.h"
#include "game/progress/ProgressStat.h"
#include "game/merchant/MerchantSaveData.h"
#include "game/mission/state/MissionState.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "game/TitleDefs.h"
#include "game/EndGame/EndGameContent.h"
#include "game/reward/RewardData.h"
#include "CharacterSaveData.generated.h"

struct EnchantmentSaveData {
	EEnchantmentTypeID id;
	int32 level;
};

// Keep as struct in case we need to add more data
struct ArmorPropertySaveData {
	ArmorPropertySaveData() {
	}

	ArmorPropertySaveData(const FArmorPropertyData inData)
		: id(inData.ID)
		, rarity(inData.Rarity) {
	}

	EArmorPropertyID id;
	EItemRarity rarity;
};

struct ItemSaveData {
	ItemSaveData(const FItemId& id):type(id){};
	ItemSaveData(const FItemId&, float power, EItemRarity, bool upgraded, bool gifted, bool modified, bool equipped, bool markedNew, bool cloned, EEquipmentSlot, int32 index, std::vector<EnchantmentSaveData>, std::vector<ArmorPropertySaveData>, TOptional<EnchantmentSaveData> netheriteEnchant);
	FItemId type;
	float power;
	EItemRarity rarity;
	bool upgraded;
	bool gifted;
	bool modified;
	bool equipped;
	bool markedNew; //Needs to be saved since we are especially interested in the 'picking up loot during level, and seeing whats new in lobby' case.
	bool cloned; //Clones items should not have value and they should not be giftable.
	EEquipmentSlot equipmentSlot;
	int32 inventoryIndex;
	std::vector<EnchantmentSaveData> enchantments;
	std::vector<ArmorPropertySaveData> armorProperties;
	TOptional<EnchantmentSaveData> netheriteEnchant;

	EEquipmentSlot GetEquipmentSlot() const {
		if (equipped) {
			return equipmentSlot;
		}

		return EEquipmentSlot::Invalid;
	}

	uint32 GetIndex() const {
		return inventoryIndex;
	}

	bool Equipped() const {
		return equipped;
	}
};

struct CurrencySaveData {
	CurrencySaveData(const FItemId& id, uint32 inCount) : type(id), count(inCount) {};
	FItemId type;
	uint32 count;
};

struct MissionProgressSaveData {
	TOptional<EGameDifficulty> completedDifficulty;
	TOptional<EThreatLevel> completedThreatLevel;
	FEndlessStruggle completedEndlessStruggle;
};

struct DifficultyProgressSaveData {
	TOptional<EGameDifficulty> unlocked;
	TOptional<EGameDifficulty> announced;
	TOptional<EGameDifficulty> selected;
};

struct ThreatLevelProgressSaveData {
	TOptional<EThreatLevel> unlocked;
	TOptional<EThreatLevel> announced;
};

struct EndGameContentProgressSaveData {
	TSet<EEndGameContentType> announcedUnlockedContent;
};

struct MapUIState {
	TOptional<ERealmName> selectedRealm;
	TOptional<EGameDifficulty> selectedDifficulty;
	TOptional<ELevelNames> selectedMission;
	TOptional<EThreatLevel> selectedThreatLevel;
	TOptional<FVector2D> panPosition;
};

struct ChestSaveData {
	uint32 unlockedTimes;
	ChestSaveData() : unlockedTimes(0) {}
};

struct UIHintProgressSaveData {
	EUIHintType hintType;
};

struct CompletedTrialData {
	FString id;
	EGameDifficulty difficulty;

	bool operator==(const CompletedTrialData& rhs) const {
		return id == rhs.id && difficulty == rhs.difficulty;
	};
};

struct CosmeticsSaveData {

	CosmeticsSaveData(): type(ECosmeticType::Unset) {}

	CosmeticsSaveData(FName id, ECosmeticType type)
		: id(std::move(id)),
		  type(type) {
	}

	FName id;
	ECosmeticType type;
};

struct StrongHoldProgress {
	bool unlocked = false;
	bool hasEye = false;
};

struct StrongholdProgressSaveData {
	void SetUsedPortal(bool value) { usedPortal = value; }

	int  EyesHeldCount() const;
	int  EyesPlacedInPortalCount() const;
	bool AreAllEyesPlacedInPortal() const;

	bool IsUnlocked(EEyeOfEnderType) const;
	bool HasEye(EEyeOfEnderType) const;

	bool AllowsPickup(EEyeOfEnderType) const;
	void GiveEye(EEyeOfEnderType);

	void UnlockAndRemoveEye(EEyeOfEnderType);

	TMap<EEyeOfEnderType, StrongHoldProgress> progresses;
	bool usedPortal = false;
};

// D11.SSN
UENUM(BlueprintType)
enum class ELegendaryStatus : uint8 {
	HERO,
	LEGENDARY,
	MYTHICAL
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLoadFinished, bool, bSuccessfull, int32, LocalUserNum, class UCharacterSaveData*, savedata);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveFailed, FString, filepath);

UCLASS()
class DUNGEONS_API UCharacterSaveData final : public UObject {
	GENERATED_BODY()

public:
	using MissionProgressMap = std::unordered_map<ELevelNames, MissionProgressSaveData>;

	UCharacterSaveData() = default;

	UPROPERTY(BlueprintAssignable, Category = "CharacterSaveData")
	FOnLoadFinished OnLoadFinished;

	UPROPERTY(BlueprintAssignable, Category = "CharacterSaveData")
	FOnSaveFailed OnSaveFailed;
	
	void CreateNew(FString path);
	void CreateNewDeepClone(const UCharacterSaveData& cloneSource, const FString& newSavePath  );
	void CreateNewShallowClone(const UCharacterSaveData& cloneSource, const FString& newSavePath);

	void Load(FString path, int32 localUserNum = 0);

	UFUNCTION()
	void OnConsoleSaveDataFinishedLoad(bool bSuccessfull, FString filename, int32 userIndex, TArray<uint8>& saveBlob);

	void AfterLoadSaveData(int32 LocalUserNum, Json::Value &node);

	void Save(int32 localUserNum = 0);

	bool MoveFileTo(const FString& destinationPath);
	FString GetBaseFilename();

	bool HasCompletedAnyLevel() const;

	void SetLegendaryStatus(bool legendary);

	UFUNCTION(BlueprintCallable, Category = "CharacterSaveData")
	ELegendaryStatus GetLegendaryStatus();

	struct RecordedData
	{
		FString name;
		FString creationDate;

		FName skin = NAME_None;

		bool customized = false;
		bool isClone = false;
		ELegendaryStatus legendaryStatus = ELegendaryStatus::HERO;

		int32 xp = 0;
		int32 totalGearPower = 0;
		uint32 version = 0;

		FGuid playerId;

		TArray<FRewardData> pendingRewardItems;

		std::vector<ItemSaveData> items;
		std::vector<CurrencySaveData> currency;
		std::vector<CompletedTrialData> trialsCompleted;
		std::vector<UIHintProgressSaveData> uiHintsExpired;

		TArray<CosmeticsSaveData> cosmetics;
		TArray<FString> progressionKeys;

		StrongholdProgressSaveData strongholdProgress;

		TSet<FName> cosmeticsEverEquipped;
		mutable TSet<FItemId> itemsFound;
		mutable TSet<FItemId> currenciesFound;
		TSet<ELevelNames> unlockedSecretMissions;

		UCharacterSaveData::MissionProgressMap progress;

		DifficultyProgressSaveData difficulties;
		ThreatLevelProgressSaveData threatLevels;
		EndGameContentProgressSaveData endGameContentProgress;
		MapUIState mapUIState;

		std::unordered_map<int32, ChestSaveData> unlockedLobbyChests;
		std::unordered_map<EntityType, int32> mobKills;
		std::unordered_map<std::string, int32> finishedObjectiveTags;
		TMap<EProgressStat, int32> progressStatCounters;
		TSet<FString> videosPlayed;

		void RecordSaveDataToJsonNode(Json::Value& node)const;
		void ReadSaveDataFromJsonNode(Json::Value& node);

		TMap<FName, FMerchantSaveData> merchantData;

		TMap<ELevelNames, FMissionState> missionStates;
	};
	RecordedData mRecordedData;

	FGuid GetPlayerId() const
	{
		return mRecordedData.playerId;
	}
	void SetPlayerId(FGuid Guid) 
	{
		mRecordedData.playerId = Guid;
	}
	void SetNewPlayerId();

	FString mSavePathAndFilename;
	FString mLastFailedFileWrite;

	bool markedForDelete = false;

	void addTrialCompleted(const FString& trialId, EGameDifficulty);
	bool hasCompletedTrial(const FString& trialId, EGameDifficulty) const;
	bool clearTrial(const FString& trialId, EGameDifficulty);
	void clearTrials();

	void addPendingRewardItem(const FRewardData& item);
	const FRewardData& getNextPendingRewardItem();
	ERewardType getNextPendingRewardItemRewardType();
	TOptional<FRewardData> claimNextPendingRewardItem();
	bool hasPendingRewardItem() const;
	int numPendingRewards() const;
	uint32 getCurrencyFor(const FItemId& inType) const;
	TArray<const FItemId*> getOwnedCurrencyTypes() const;
	FString getCurrentDate(); // D11.SSN

	FName GenerateRandomDefaultSkin() const;


	static void EnsurePathExists(FString inSavePathAndFilename);


#if CONSOLE_SAVE_SYSTEM
private:
	FDelegateHandle ConsoleSaveFinishedLoadingDelegate;
#endif
};
