// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_GameplayAbilityTargetTypes_generated_h
#error "GameplayAbilityTargetTypes.generated.h already included, missing '#pragma once' in GameplayAbilityTargetTypes.h"
#endif
#define GAMEPLAYABILITIES_GameplayAbilityTargetTypes_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetTypes_h_563_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityTargetData_SingleTargetHit_Statics; \
	static class UScriptStruct* StaticStruct(); \
	typedef FGameplayAbilityTargetData Super;


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityTargetData_SingleTargetHit>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetTypes_h_460_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityTargetData_ActorArray_Statics; \
	static class UScriptStruct* StaticStruct(); \
	typedef FGameplayAbilityTargetData Super;


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityTargetData_ActorArray>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetTypes_h_398_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityTargetData_LocationInfo_Statics; \
	static class UScriptStruct* StaticStruct(); \
	typedef FGameplayAbilityTargetData Super;


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityTargetData_LocationInfo>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetTypes_h_299_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityTargetingLocationInfo_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityTargetingLocationInfo>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetTypes_h_193_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityTargetDataHandle_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityTargetDataHandle>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetTypes_h_68_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityTargetData_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityTargetData>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTargetTypes_h


#define FOREACH_ENUM_EABILITYGENERICREPLICATEDEVENT(op) \
	op(EAbilityGenericReplicatedEvent::GenericConfirm) \
	op(EAbilityGenericReplicatedEvent::GenericCancel) \
	op(EAbilityGenericReplicatedEvent::InputPressed) \
	op(EAbilityGenericReplicatedEvent::InputReleased) \
	op(EAbilityGenericReplicatedEvent::GenericSignalFromClient) \
	op(EAbilityGenericReplicatedEvent::GenericSignalFromServer) \
	op(EAbilityGenericReplicatedEvent::GameCustom1) \
	op(EAbilityGenericReplicatedEvent::GameCustom2) \
	op(EAbilityGenericReplicatedEvent::GameCustom3) \
	op(EAbilityGenericReplicatedEvent::GameCustom4) \
	op(EAbilityGenericReplicatedEvent::GameCustom5) \
	op(EAbilityGenericReplicatedEvent::GameCustom6) 
#define FOREACH_ENUM_EGAMEPLAYABILITYTARGETINGLOCATIONTYPE(op) \
	op(EGameplayAbilityTargetingLocationType::LiteralTransform) \
	op(EGameplayAbilityTargetingLocationType::ActorTransform) \
	op(EGameplayAbilityTargetingLocationType::SocketTransform) 
#define FOREACH_ENUM_EGAMEPLAYTARGETINGCONFIRMATION(op) \
	op(EGameplayTargetingConfirmation::Instant) \
	op(EGameplayTargetingConfirmation::UserConfirmed) \
	op(EGameplayTargetingConfirmation::Custom) \
	op(EGameplayTargetingConfirmation::CustomMulti) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
