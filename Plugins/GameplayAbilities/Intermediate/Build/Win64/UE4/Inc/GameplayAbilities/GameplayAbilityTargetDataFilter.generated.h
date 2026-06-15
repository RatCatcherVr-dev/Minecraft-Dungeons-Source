// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_GameplayAbilityTargetDataFilter_generated_h
#error "GameplayAbilityTargetDataFilter.generated.h already included, missing '#pragma once' in GameplayAbilityTargetDataFilter.h"
#endif
#define GAMEPLAYABILITIES_GameplayAbilityTargetDataFilter_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetDataFilter_h_87_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayTargetDataFilterHandle>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetDataFilter_h_27_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayTargetDataFilter>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetDataFilter_h


#define FOREACH_ENUM_ETARGETDATAFILTERSELF(op) \
	op(ETargetDataFilterSelf::TDFS_Any) \
	op(ETargetDataFilterSelf::TDFS_NoSelf) \
	op(ETargetDataFilterSelf::TDFS_NoOthers) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
