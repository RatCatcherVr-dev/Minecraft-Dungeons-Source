// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_AbilitySystemGlobals_generated_h
#error "AbilitySystemGlobals.generated.h already included, missing '#pragma once' in AbilitySystemGlobals.h"
#endif
#define GAMEPLAYABILITIES_AbilitySystemGlobals_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execToggleIgnoreAbilitySystemCosts) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ToggleIgnoreAbilitySystemCosts(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execToggleIgnoreAbilitySystemCooldowns) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ToggleIgnoreAbilitySystemCooldowns(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execToggleIgnoreAbilitySystemCosts) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ToggleIgnoreAbilitySystemCosts(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execToggleIgnoreAbilitySystemCooldowns) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ToggleIgnoreAbilitySystemCooldowns(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilitySystemGlobals(); \
	friend struct Z_Construct_UClass_UAbilitySystemGlobals_Statics; \
public: \
	DECLARE_CLASS(UAbilitySystemGlobals, UObject, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilitySystemGlobals) \
	static const TCHAR* StaticConfigName() {return TEXT("Game");} \



#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_INCLASS \
private: \
	static void StaticRegisterNativesUAbilitySystemGlobals(); \
	friend struct Z_Construct_UClass_UAbilitySystemGlobals_Statics; \
public: \
	DECLARE_CLASS(UAbilitySystemGlobals, UObject, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilitySystemGlobals) \
	static const TCHAR* StaticConfigName() {return TEXT("Game");} \



#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilitySystemGlobals) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilitySystemGlobals); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilitySystemGlobals); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilitySystemGlobals(UAbilitySystemGlobals&&); \
	NO_API UAbilitySystemGlobals(const UAbilitySystemGlobals&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilitySystemGlobals(UAbilitySystemGlobals&&); \
	NO_API UAbilitySystemGlobals(const UAbilitySystemGlobals&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilitySystemGlobals); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilitySystemGlobals); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilitySystemGlobals)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__bAllowGameplayModEvaluationChannels() { return STRUCT_OFFSET(UAbilitySystemGlobals, bAllowGameplayModEvaluationChannels); } \
	FORCEINLINE static uint32 __PPO__DefaultGameplayModEvaluationChannel() { return STRUCT_OFFSET(UAbilitySystemGlobals, DefaultGameplayModEvaluationChannel); } \
	FORCEINLINE static uint32 __PPO__GameplayModEvaluationChannelAliases() { return STRUCT_OFFSET(UAbilitySystemGlobals, GameplayModEvaluationChannelAliases); } \
	FORCEINLINE static uint32 __PPO__GlobalCurveTableName() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalCurveTableName); } \
	FORCEINLINE static uint32 __PPO__GlobalCurveTable() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalCurveTable); } \
	FORCEINLINE static uint32 __PPO__GlobalAttributeMetaDataTableName() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalAttributeMetaDataTableName); } \
	FORCEINLINE static uint32 __PPO__GlobalAttributeMetaDataTable() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalAttributeMetaDataTable); } \
	FORCEINLINE static uint32 __PPO__GlobalAttributeSetDefaultsTableName() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalAttributeSetDefaultsTableName); } \
	FORCEINLINE static uint32 __PPO__GlobalAttributeSetDefaultsTableNames() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalAttributeSetDefaultsTableNames); } \
	FORCEINLINE static uint32 __PPO__GlobalAttributeDefaultsTables() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalAttributeDefaultsTables); } \
	FORCEINLINE static uint32 __PPO__GlobalGameplayCueManagerClass() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalGameplayCueManagerClass); } \
	FORCEINLINE static uint32 __PPO__GlobalGameplayCueManagerName() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalGameplayCueManagerName); } \
	FORCEINLINE static uint32 __PPO__GameplayCueNotifyPaths() { return STRUCT_OFFSET(UAbilitySystemGlobals, GameplayCueNotifyPaths); } \
	FORCEINLINE static uint32 __PPO__GameplayTagResponseTableName() { return STRUCT_OFFSET(UAbilitySystemGlobals, GameplayTagResponseTableName); } \
	FORCEINLINE static uint32 __PPO__GameplayTagResponseTable() { return STRUCT_OFFSET(UAbilitySystemGlobals, GameplayTagResponseTable); } \
	FORCEINLINE static uint32 __PPO__PredictTargetGameplayEffects() { return STRUCT_OFFSET(UAbilitySystemGlobals, PredictTargetGameplayEffects); } \
	FORCEINLINE static uint32 __PPO__GlobalGameplayCueManager() { return STRUCT_OFFSET(UAbilitySystemGlobals, GlobalGameplayCueManager); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_27_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h_30_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilitySystemGlobals."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilitySystemGlobals>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemGlobals_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
