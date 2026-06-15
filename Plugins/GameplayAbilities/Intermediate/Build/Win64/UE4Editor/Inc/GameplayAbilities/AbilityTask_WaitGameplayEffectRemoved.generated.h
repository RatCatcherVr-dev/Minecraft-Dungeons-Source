// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayEffectRemovalInfo;
class UGameplayAbility;
struct FActiveGameplayEffectHandle;
class UAbilityTask_WaitGameplayEffectRemoved;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitGameplayEffectRemoved_generated_h
#error "AbilityTask_WaitGameplayEffectRemoved.generated.h already included, missing '#pragma once' in AbilityTask_WaitGameplayEffectRemoved.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitGameplayEffectRemoved_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_10_DELEGATE \
struct _Script_GameplayAbilities_eventWaitGameplayEffectRemovedDelegate_Parms \
{ \
	FGameplayEffectRemovalInfo GameplayEffectRemovalInfo; \
}; \
static inline void FWaitGameplayEffectRemovedDelegate_DelegateWrapper(const FMulticastScriptDelegate& WaitGameplayEffectRemovedDelegate, FGameplayEffectRemovalInfo const& GameplayEffectRemovalInfo) \
{ \
	_Script_GameplayAbilities_eventWaitGameplayEffectRemovedDelegate_Parms Parms; \
	Parms.GameplayEffectRemovalInfo=GameplayEffectRemovalInfo; \
	WaitGameplayEffectRemovedDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitForGameplayEffectRemoved) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitGameplayEffectRemoved**)Z_Param__Result=UAbilityTask_WaitGameplayEffectRemoved::WaitForGameplayEffectRemoved(Z_Param_OwningAbility,Z_Param_Handle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnGameplayEffectRemoved) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectRemovalInfo,Z_Param_Out_InGameplayEffectRemovalInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnGameplayEffectRemoved(Z_Param_Out_InGameplayEffectRemovalInfo); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitForGameplayEffectRemoved) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitGameplayEffectRemoved**)Z_Param__Result=UAbilityTask_WaitGameplayEffectRemoved::WaitForGameplayEffectRemoved(Z_Param_OwningAbility,Z_Param_Handle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnGameplayEffectRemoved) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectRemovalInfo,Z_Param_Out_InGameplayEffectRemovalInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnGameplayEffectRemoved(Z_Param_Out_InGameplayEffectRemovalInfo); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitGameplayEffectRemoved(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitGameplayEffectRemoved_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitGameplayEffectRemoved, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitGameplayEffectRemoved)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitGameplayEffectRemoved(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitGameplayEffectRemoved_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitGameplayEffectRemoved, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitGameplayEffectRemoved)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitGameplayEffectRemoved(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitGameplayEffectRemoved) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitGameplayEffectRemoved); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitGameplayEffectRemoved); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitGameplayEffectRemoved(UAbilityTask_WaitGameplayEffectRemoved&&); \
	NO_API UAbilityTask_WaitGameplayEffectRemoved(const UAbilityTask_WaitGameplayEffectRemoved&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitGameplayEffectRemoved(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitGameplayEffectRemoved(UAbilityTask_WaitGameplayEffectRemoved&&); \
	NO_API UAbilityTask_WaitGameplayEffectRemoved(const UAbilityTask_WaitGameplayEffectRemoved&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitGameplayEffectRemoved); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitGameplayEffectRemoved); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitGameplayEffectRemoved)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_18_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h_21_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitGameplayEffectRemoved."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitGameplayEffectRemoved>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectRemoved_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
