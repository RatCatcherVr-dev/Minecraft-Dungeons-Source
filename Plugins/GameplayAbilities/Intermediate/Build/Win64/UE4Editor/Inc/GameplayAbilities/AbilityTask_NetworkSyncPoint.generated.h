// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
enum class EAbilityTaskNetSyncType : uint8;
class UAbilityTask_NetworkSyncPoint;
#ifdef GAMEPLAYABILITIES_AbilityTask_NetworkSyncPoint_generated_h
#error "AbilityTask_NetworkSyncPoint.generated.h already included, missing '#pragma once' in AbilityTask_NetworkSyncPoint.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_NetworkSyncPoint_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_9_DELEGATE \
static inline void FNetworkSyncDelegate_DelegateWrapper(const FMulticastScriptDelegate& NetworkSyncDelegate) \
{ \
	NetworkSyncDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitNetSync) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_ENUM(EAbilityTaskNetSyncType,Z_Param_SyncType); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_NetworkSyncPoint**)Z_Param__Result=UAbilityTask_NetworkSyncPoint::WaitNetSync(Z_Param_OwningAbility,EAbilityTaskNetSyncType(Z_Param_SyncType)); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnSignalCallback) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnSignalCallback(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitNetSync) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_ENUM(EAbilityTaskNetSyncType,Z_Param_SyncType); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_NetworkSyncPoint**)Z_Param__Result=UAbilityTask_NetworkSyncPoint::WaitNetSync(Z_Param_OwningAbility,EAbilityTaskNetSyncType(Z_Param_SyncType)); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnSignalCallback) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnSignalCallback(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_NetworkSyncPoint(); \
	friend struct Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_NetworkSyncPoint, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_NetworkSyncPoint)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_NetworkSyncPoint(); \
	friend struct Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_NetworkSyncPoint, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_NetworkSyncPoint)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_NetworkSyncPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_NetworkSyncPoint) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_NetworkSyncPoint); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_NetworkSyncPoint); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_NetworkSyncPoint(UAbilityTask_NetworkSyncPoint&&); \
	NO_API UAbilityTask_NetworkSyncPoint(const UAbilityTask_NetworkSyncPoint&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_NetworkSyncPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_NetworkSyncPoint(UAbilityTask_NetworkSyncPoint&&); \
	NO_API UAbilityTask_NetworkSyncPoint(const UAbilityTask_NetworkSyncPoint&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_NetworkSyncPoint); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_NetworkSyncPoint); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_NetworkSyncPoint)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_25_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h_28_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_NetworkSyncPoint."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_NetworkSyncPoint>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_NetworkSyncPoint_h


#define FOREACH_ENUM_EABILITYTASKNETSYNCTYPE(op) \
	op(EAbilityTaskNetSyncType::BothWait) \
	op(EAbilityTaskNetSyncType::OnlyServerWait) \
	op(EAbilityTaskNetSyncType::OnlyClientWait) 

enum class EAbilityTaskNetSyncType : uint8;
template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EAbilityTaskNetSyncType>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
