// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Dungeons.h"
#include "assets/ItemAssetFinder.h"
#include "Assets/MasterAssetFinder.h"
#include "locale/LocTableFromFile.h"
#include "StringTableRegistry.h"

class FDungeonsModule : public IDungeonsModule {
public:
	
	void OnPostEngineInit() {
		InitAssetFinders();
	}
	
	void StartupModule() {
		FCoreDelegates::OnPostEngineInit.AddRaw(this, &FDungeonsModule::OnPostEngineInit);

		LocTableFromFile::LoadCsvs();
	}
	
	bool IsGameModule() const override
	{
		return true;
	}

	void InitAssetFinders(bool force = false) override {
		GetMaster()->Initialize(force);
	}

	UItemAssetFinder* GetItemAssetFinder() override {
		return GetMaster()->GetItemAssetFinder();
	}


	UEnchantmentAssetFinder* GetEnchantmentAssetFinder() override {
		return GetMaster()->GetEnchantmentAssetFinder();
	}

	UArmorPropertyAssetFinder* GetArmorPropertyAssetFinder() override {
		return GetMaster()->GetArmorPropertyAssetFinder();
	}

	UAmbienceFinder* GetAmbienceFinder() override {
		 return GetMaster()->GetAmbienceFinder();
	}

	UMasterAssetFinder* GetMaster() {
		if (!AssetFinders) {
			AssetFinders = NewObject<UMasterAssetFinder>(GetTransientPackage(), UMasterAssetFinder::StaticClass(), NAME_None);
			AssetFinders->AddToRoot();
		}

		return AssetFinders;
	}

	UMasterAssetFinder* AssetFinders = nullptr;

};


IMPLEMENT_PRIMARY_GAME_MODULE(FDungeonsModule, Dungeons, "Dungeons" );

DEFINE_LOG_CATEGORY(LogDungeons)
DEFINE_LOG_CATEGORY(LogEnchantments)
DEFINE_LOG_CATEGORY(LogTelemetry)
DEFINE_LOG_CATEGORY(LogMultiplayer)
DEFINE_LOG_CATEGORY(LogDungeonsNetwork)
DEFINE_LOG_CATEGORY(LogDungeonsAPI)
DEFINE_LOG_CATEGORY(LogDungeonsAI)
DEFINE_LOG_CATEGORY(LogDungeonsEntitlements)
DEFINE_LOG_CATEGORY(LogDungeonsAudio)
