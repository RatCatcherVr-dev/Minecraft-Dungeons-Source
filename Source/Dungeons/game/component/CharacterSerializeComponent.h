#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "save/CharacterSaveData.h"
#include "game/item/InventoryItemData.h"
#include "game/progress/ProgressStat.h"
#include "game/merchant/MerchantDef.h"
#include "game/mission/state/MissionState.h"
#include "CharacterSerializeComponent.generated.h"

UCLASS()
class DUNGEONS_API UCharacterSerializeComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UCharacterSerializeComponent();


	UFUNCTION(BlueprintCallable)
	void AssignCharacter(UCharacterSaveData* character);

	UFUNCTION(BlueprintCallable)
	bool HasProfile() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FString& ReadName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetName(FString name);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FName& ReadSkin() const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetSkin(FName skinId);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FGuid GetCloudPlayerId();




	uint32 ReadCurrencyFor(const FItemId& inType) const;
	void SetCurrencyFor(const FItemId& inType, uint32 inValue);
	TArray<const FItemId*> ReadOwnedCurrencyTypes() const;

	int32 ReadMobKillsForType(EntityType inType) const;
	void AddMobKillForType(EntityType inType);

	void ChestUnlocked(int32 chestId);
	int32 GetChestUnlockedTimes(int32 chestId) const;

	UCharacterSaveData::MissionProgressMap& GetProgress();
	const UCharacterSaveData::MissionProgressMap& ReadProgress() const;
	void SetProgress(const UCharacterSaveData::MissionProgressMap& inProgress);

	const StrongholdProgressSaveData& GetStrongholdData() const;
	void SetStrongholdData(StrongholdProgressSaveData strongholdData);
	void SetStrongholdUsedPortal();
	bool HasUsedPortal() const;
	bool HasVideoBeenPlayed(const class UMediaSource&) const;
	void SetVideoWasPlayed(const class UMediaSource&);

	TOptional<game::FDifficulty> GetHighestCompletedDifficulty() const;

	bool IsDifficultyCompleted(EGameDifficulty difficulty, EThreatLevel threat) const;

	bool IsThreatLevelCompleted(EThreatLevel) const;

	bool IsLevelCompleted(FMissionDifficulty missionDifficulty) const;

	const DifficultyProgressSaveData& ReadDifficulties() const;
	void SetDifficulties(DifficultyProgressSaveData difficulties);

	const ThreatLevelProgressSaveData& ReadThreatLevels() const;
	void SetThreatLevelProgress(const ThreatLevelProgressSaveData& threatLevel);

	const EndGameContentProgressSaveData& ReadEndGameContentProgress() const;
	void SetEndGameContentProgress(const EndGameContentProgressSaveData& endGameContentProgress);

	const TSet<ELevelNames>& GetUnlockedSecretMissions() const;
	void SetUnlockedSecretMissions(const TSet<ELevelNames>& unlockedSecretMissions);

	int GetProgressStat(EProgressStat stat) const;
	void IncrementProgressStat(EProgressStat stat, int count = 1);

	const TArray<CosmeticsSaveData>& GetCosmetics() const;
	void SetCosmetics(const TArray<CosmeticsSaveData>& cosmetics);

	void SetCosmeticEquipped(FName cosmeticId);
	bool HasEverEquippedCosmetic(FName cosmeticId) const;

	const std::unordered_map<std::string, int32>& GetFinishedObjectiveTags() const;
	void FinishedObjectiveTag(const std::string& tagName);
	void IncrementObjectiveTag(const std::string& tagName, int count = 1);
	int GetFinishedObjectiveTagCount(const std::string& tagName) const;

	MapUIState& GetMapUIState();

	std::vector<ItemSaveData>& GetItems();
	void GetItemsCopy(std::vector<ItemSaveData>& Dest)const;
	const TSet<FItemId>& ReadItemsFound() const;
	TSet<FItemId>& GetItemsFound();

	const TSet<FItemId>& ReadCurrenciesFound() const;
	TSet<FItemId>& GetCurrenciesFound();

	int32 ReadXP() const;
	void SetXP(int32 inXp);

	const FGuid& ReadPlayerGuid() const;
	std::vector<UIHintProgressSaveData>& GetExpiredUiHints();

	void AddPendingRewardItem(const FRewardData& item);

	const TArray<FString>& GetUnlockKeys() const;
	bool AddUnlockKey(const FString&);
	bool RemoveUnlockKey(const FString&);
	bool HasUnlockKey(const FString&) const;

	bool HasPendingRewardItem() const;
	int NumPendingRewards() const;
	TOptional<FRewardData> ClaimPendingRewardItem();

	//HasPendingRewardItem must return true for this call to be valid!
	const FRewardData& PeekPendingReward() const;

	bool UnlockMerchant(const TSubclassOf<UMerchantDef> merchant);
	bool HasUnlockedMerchant(const TSubclassOf<UMerchantDef> merchant) const;

	void AddTrialCompleted(const FString& trialId, EGameDifficulty);
	bool HasCompletedTrial(const FString& trialId, EGameDifficulty) const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetLegendaryStatus(ELegendaryStatus newStatus);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ELegendaryStatus GetLegendaryStatus();

	//Hyper-Mission
	bool HasMissionState(ELevelNames) const;
	bool ClearMissionState(ELevelNames, const FString& guid);
	bool ForceClearMissionState(ELevelNames);
	void SetMissionState(ELevelNames, const FMissionState&);
	const FMissionState* ReadMissionState(ELevelNames) const;
	FMissionState* EditMissionState(ELevelNames);
	FMissionState* EditMissionState(ELevelNames, const FString& guid);

	void ClearTrial_DEBUG(const FString& trialId, EGameDifficulty);
	void ClearTrials_DEBUG();

	bool HasMechantSaveData(const FName& merchantId);
	void CreateMechantSaveData(const FName& merchantId);
	const FMerchantSaveData& ReadMechantSaveData(const FName& merchantId); // MS.DG.POTATO
	FMerchantSaveData& EditMechantSaveData(const FName& merchantId);

	UCharacterSaveData* GetSaveData() const;

	const EndGameContentProgressSaveData& ReadAnnouncedEndGameContent() const;

	void EndPlay(EEndPlayReason::Type) override;
	void BeginPlay() override;
	
	void InvalidateSaveData() { mSaveData = nullptr; };

protected:
	virtual void IncrementDirtyFlag(int32 inValue) {};
	virtual void OnCharacterAssigned() {};

	UPROPERTY()
	UCharacterSaveData* mDefaultSaveData = nullptr;

	UPROPERTY()
	UCharacterSaveData* mSaveData = nullptr;
};
