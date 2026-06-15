// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
struct FGameplayTagQuery;
class UAbilityTask_WaitAbilityCommit;
struct FGameplayTag;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitAbilityCommit_generated_h
#error "AbilityTask_WaitAbilityCommit.generated.h already included, missing '#pragma once' in AbilityTask_WaitAbilityCommit.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitAbilityCommit_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_10_DELEGATE \
struct _Script_GameplayAbilities_eventWaitAbilityCommitDelegate_Parms \
{ \
	UGameplayAbility* ActivatedAbility; \
}; \
static inline void FWaitAbilityCommitDelegate_DelegateWrapper(const FMulticastScriptDelegate& WaitAbilityCommitDelegate, UGameplayAbility* ActivatedAbility) \
{ \
	_Script_GameplayAbilities_eventWaitAbilityCommitDelegate_Parms Parms; \
	Parms.ActivatedAbility=ActivatedAbility; \
	WaitAbilityCommitDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitForAbilityCommit_Query) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTagQuery,Z_Param_Query); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAbilityCommit**)Z_Param__Result=UAbilityTask_WaitAbilityCommit::WaitForAbilityCommit_Query(Z_Param_OwningAbility,Z_Param_Query,Z_Param_TriggerOnce); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWaitForAbilityCommit) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithTag); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithoutTage); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAbilityCommit**)Z_Param__Result=UAbilityTask_WaitAbilityCommit::WaitForAbilityCommit(Z_Param_OwningAbility,Z_Param_WithTag,Z_Param_WithoutTage,Z_Param_TriggerOnce); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnAbilityCommit) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_ActivatedAbility); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnAbilityCommit(Z_Param_ActivatedAbility); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitForAbilityCommit_Query) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTagQuery,Z_Param_Query); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAbilityCommit**)Z_Param__Result=UAbilityTask_WaitAbilityCommit::WaitForAbilityCommit_Query(Z_Param_OwningAbility,Z_Param_Query,Z_Param_TriggerOnce); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWaitForAbilityCommit) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithTag); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithoutTage); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAbilityCommit**)Z_Param__Result=UAbilityTask_WaitAbilityCommit::WaitForAbilityCommit(Z_Param_OwningAbility,Z_Param_WithTag,Z_Param_WithoutTage,Z_Param_TriggerOnce); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnAbilityCommit) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_ActivatedAbility); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnAbilityCommit(Z_Param_ActivatedAbility); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAbilityCommit(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAbilityCommit_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAbilityCommit, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAbilityCommit)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAbilityCommit(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAbilityCommit_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAbilityCommit, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAbilityCommit)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAbilityCommit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAbilityCommit) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAbilityCommit); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAbilityCommit); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAbilityCommit(UAbilityTask_WaitAbilityCommit&&); \
	NO_API UAbilityTask_WaitAbilityCommit(const UAbilityTask_WaitAbilityCommit&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAbilityCommit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAbilityCommit(UAbilityTask_WaitAbilityCommit&&); \
	NO_API UAbilityTask_WaitAbilityCommit(const UAbilityTask_WaitAbilityCommit&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAbilityCommit); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAbilityCommit); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAbilityCommit)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_15_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h_18_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitAbilityCommit."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitAbilityCommit>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAbilityCommit_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
