#pragma once

#include "CharacterSaveData.h"
#include "game/actor/character/player/PlayerCharacterSaveSlot.h"
#include "game/input/KeyBinding.h" // D11.DB
#include "SettingsBlueprintFunctionLibrary.h" //D11.LG
#include "StatTracker.h" // D11.SSN
#include "game/SaveConstants.h"
#include "StaticArray.h"
#include "DungeonsConsoleSave.h" // D11.SSN
#include "online/entitlements/Entitlement.h"

#include "GlobalSaveData.generated.h"

struct ReconnectSaveData {
	FString sessionId;	// last session we've connected to
	FGuid guid;		// identifier for 'device' for reconnecting

	void set(const FString& sid) {
		sessionId = sid;
	}

	void clear() {
		sessionId = "";
	}

	bool isSet() const {
		return !sessionId.IsEmpty();
	}
};

#define LOAD_OLDAPPDATA_SAVES 1

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGlobalSaveDataLoaded, UGlobalStateData*, saveData, int32, UserIndex);

enum ECharacterCloneType
{
	Deep,
	Shallow
};

UCLASS()
class UGlobalStateData : public UObject {
	GENERATED_BODY()

public:
	UGlobalStateData();
	virtual ~UGlobalStateData();
	void CreateNewState();

	void Load(int32 localUserNum);
	void AddProfilesInPath(const FString& inPath, int32 localUserNum);

	UPROPERTY(BlueprintAssignable)
	FOnGlobalSaveDataLoaded OnGlobalSaveDataLoaded;
	UFUNCTION()
	void AfterConsoleSaveDataLoaded(bool bSuccessfull, FString filename, int32 localUserNum, TArray<uint8> &saveBlob);
	
	
	FDelegateHandle GlobalSaveDataLoadDelegate;
	void AfterLoadGlobalSaveData(Json::Value &root, int32 localUserNum);

	void Save(int32 localUserNum) const;

	bool DeleteByIndex(int32 localUserNum, int32 inIndex);

	void LoadAllCharacterProfiles(int32 localUserNum);
	FString GetRecentSaveDataByKey(const FString& key) const;
	void SetRecentSaveData(const FString& key, const FString& filename);
	UCharacterSaveData* CloneCharacterProfile(const UCharacterSaveData& cloneSource, int32 localUserNum , ECharacterCloneType CloneType = ECharacterCloneType::Shallow );
	UCharacterSaveData* CreateNewCharacterProfile(int32 localUserNum);
	TArray<UCharacterSaveData*> GetAllProfiles();

	void LoadCharacterSaveData(FString path, int32 localUserNum = 0);
	UFUNCTION()
	void OnCharacterLoadFinished(bool bSuccessfull, int32 LocalUserNum, UCharacterSaveData* saveData);

	const ReconnectSaveData& ReadReconnectState() const;
	ReconnectSaveData& GetReconnectState();

	// D11.DB
	TArray<FKeybinding>& GetKeybinds();

	// D11.LG
	Json::Value& GetSettingsNode();

	// D11.SSN
	TrackedStats& GetTrackedStats();

	Json::Value& GetTrackedStatsNode();

	bool HasSelectedSkinId(const FName&) const;
	void AddSelectedSkinId(const FName&);

	bool HasSeenNewsId(const FString& id) const;
	void AddSeenNewsId(const FString& id);

	int32 GetIndexForFilename(const FString& filename) const;
	APlayerCharacterSaveSlot* GetCharacterSlotFor(UObject* WorldContextObject, UCharacterSaveData* saveData, bool forceRefreshSlot = false);

	// paths:
	FString GetProfilePath(uint32 currentPlayerIndex) const;
	FString GetGlobalSavePath(uint32 currentPlayerIndex) const;
	FString GetOldSavePath(uint32 currentPlayerIndex) const;
	FString GetCharacterSavePath(uint32 currentPlayerIndex) const;
	FString GetOldCharacterSavePath(uint32 currentPlayerIndex) const;
	FString GetDeletedCharacterSavePath(uint32 currentPlayerIndex) const;
	FString GenerateCharacterFilename(uint32 currentPlayerIndex) const; // #D11.CM (and some other peeps)
	bool IsInitialBootFlowComplete() const;
	void SetInitialBootFlowComplete();
	void SetPIESaveFolder(std::string prefix);
	FString  GetGlobalSaveFileName() const;
	
	bool mNewGlobalSave = false;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ETitle GetTitle();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ChangeTitle(ETitle title);

	std::string mPIESaveFolder;

	uint32 mVersion = 0;

	struct RecordedData
	{
		FString mLocale;
		ReconnectSaveData mReconnect;

		TMap<FString, FString> mRecentSaveFilenames;

		// D11.DB
		TArray<FKeybinding> mKeybinds;

		// D11.LG
		Json::Value mSettingsNode;
		
		// D11.SSN
		TrackedStats mTrackedStats;
		
		TSet<FName> mSkinsSelected;

		TArray<FString> mSeenNews;

		bool mAccountLinked = false;

		bool mCrossplay = true;

		bool mInitialBootFlowComplete = true;

		bool mXblActive = true;

		uint8 SelectedGameMode = 1u;

		bool mCloudNag = false;

		void RecordGlobalSaveDataToJsonNode(Json::Value& node)const;

		ETitle title = ETitle::NONE;

		TArray<FEntitlement> mCachedEntitlements;
	};
	RecordedData mRecordedData;

	Json::Value mTrackStatsNode;

	UPROPERTY()
	TArray<UCharacterSaveData*> mSlots;

	UPROPERTY()
	TMap<FString, TWeakObjectPtr<APlayerCharacterSaveSlot>> mCharacterSlots;


	uint8 CharacterSlotsLeftToCheck;

private:
};

namespace globalsavedata {
	const FString BinaryFileExtension = ".dat";
}


#define CONSOLE_GLOBAL_SAVE_FILE FString(TEXT("GlobalSave"))
