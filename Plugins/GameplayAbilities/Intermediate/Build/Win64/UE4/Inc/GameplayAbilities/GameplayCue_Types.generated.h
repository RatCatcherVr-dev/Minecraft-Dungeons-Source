// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_GameplayCue_Types_generated_h
#error "GameplayCue_Types.generated.h already included, missing '#pragma once' in GameplayCue_Types.h"
#endif
#define GAMEPLAYABILITIES_GameplayCue_Types_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCue_Types_h_73_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FPreallocationInfo_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FPreallocationInfo>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCue_Types_h_36_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayCuePendingExecute>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCue_Types_h


#define FOREACH_ENUM_EGAMEPLAYCUEPAYLOADTYPE(op) \
	op(EGameplayCuePayloadType::EffectContext) \
	op(EGameplayCuePayloadType::CueParameters) \
	op(EGameplayCuePayloadType::FromSpec) 

enum class EGameplayCuePayloadType : uint8;
template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EGameplayCuePayloadType>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
