// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#ifndef __DUNGEONS_H__
#define __DUNGEONS_H__

#include "EngineMinimal.h"
#include <MultiBoxDefs.h>	// included before we enable warning C4062 as error
#include "CompilationResult.h"
#include "common_header.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDungeons, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEnchantments, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogTelemetry, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMultiplayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsNetwork, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsAPI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsEntitlements, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsAudio, Log, All);

class IDungeonsModule : public IModuleInterface {
public:
	static inline IDungeonsModule& Get() {
		static IDungeonsModule& Singleton = FModuleManager::LoadModuleChecked< IDungeonsModule >("Dungeons");
		return Singleton;
	}

	virtual class UItemAssetFinder* GetItemAssetFinder() = 0;
	virtual class UEnchantmentAssetFinder* GetEnchantmentAssetFinder() = 0;
	virtual class UArmorPropertyAssetFinder* GetArmorPropertyAssetFinder() = 0;
	virtual class UAmbienceFinder* GetAmbienceFinder() = 0;
	
	virtual void InitAssetFinders(bool force = false) = 0;
};


#endif
