// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_GameplayAbilityTypes_generated_h
#error "GameplayAbilityTypes.generated.h already included, missing '#pragma once' in GameplayAbilityTypes.h"
#endif
#define GAMEPLAYABILITIES_GameplayAbilityTypes_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_496_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilitySpecHandleAndPredictionKey_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilitySpecHandleAndPredictionKey>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_445_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FServerAbilityRPCBatch_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FServerAbilityRPCBatch>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_421_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FAbilityTaskDebugMessage_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FAbilityTaskDebugMessage>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_403_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FAttributeDefaults_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FAttributeDefaults>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_331_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FAbilityEndedData_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FAbilityEndedData>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_268_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayEventData_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayEventData>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_239_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityLocalAnimMontage_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityLocalAnimMontage>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_179_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityRepAnimMontage_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityRepAnimMontage>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h_118_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityActorInfo_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityActorInfo>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbilityTypes_h


#define FOREACH_ENUM_EGAMEPLAYABILITYTRIGGERSOURCE(op) \
	op(EGameplayAbilityTriggerSource::GameplayEvent) \
	op(EGameplayAbilityTriggerSource::OwnedTagAdded) \
	op(EGameplayAbilityTriggerSource::OwnedTagPresent) 
#define FOREACH_ENUM_EGAMEPLAYABILITYREPLICATIONPOLICY(op) \
	op(EGameplayAbilityReplicationPolicy::ReplicateNo) \
	op(EGameplayAbilityReplicationPolicy::ReplicateYes) 
#define FOREACH_ENUM_EGAMEPLAYABILITYNETEXECUTIONPOLICY(op) \
	op(EGameplayAbilityNetExecutionPolicy::LocalPredicted) \
	op(EGameplayAbilityNetExecutionPolicy::LocalOnly) \
	op(EGameplayAbilityNetExecutionPolicy::ServerInitiated) \
	op(EGameplayAbilityNetExecutionPolicy::ServerOnly) 
#define FOREACH_ENUM_EGAMEPLAYABILITYINSTANCINGPOLICY(op) \
	op(EGameplayAbilityInstancingPolicy::NonInstanced) \
	op(EGameplayAbilityInstancingPolicy::InstancedPerActor) \
	op(EGameplayAbilityInstancingPolicy::InstancedPerExecution) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
