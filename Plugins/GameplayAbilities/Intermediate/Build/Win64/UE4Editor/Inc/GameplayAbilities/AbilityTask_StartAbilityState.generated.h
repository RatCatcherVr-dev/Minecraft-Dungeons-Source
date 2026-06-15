// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
class UAbilityTask_StartAbilityState;
#ifdef GAMEPLAYABILITIES_AbilityTask_StartAbilityState_generated_h
#error "AbilityTask_StartAbilityState.generated.h already included, missing '#pragma once' in AbilityTask_StartAbilityState.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_StartAbilityState_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_10_DELEGATE \
static inline void FAbilityStateDelegate_DelegateWrapper(const FMulticastScriptDelegate& AbilityStateDelegate) \
{ \
	AbilityStateDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execStartAbilityState) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_StateName); \
		P_GET_UBOOL(Z_Param_bEndCurrentState); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_StartAbilityState**)Z_Param__Result=UAbilityTask_StartAbilityState::StartAbilityState(Z_Param_OwningAbility,Z_Param_StateName,Z_Param_bEndCurrentState); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execStartAbilityState) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_StateName); \
		P_GET_UBOOL(Z_Param_bEndCurrentState); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_StartAbilityState**)Z_Param__Result=UAbilityTask_StartAbilityState::StartAbilityState(Z_Param_OwningAbility,Z_Param_StateName,Z_Param_bEndCurrentState); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_StartAbilityState(); \
	friend struct Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_StartAbilityState, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_StartAbilityState)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_StartAbilityState(); \
	friend struct Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_StartAbilityState, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_StartAbilityState)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_StartAbilityState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_StartAbilityState) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_StartAbilityState); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_StartAbilityState); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_StartAbilityState(UAbilityTask_StartAbilityState&&); \
	NO_API UAbilityTask_StartAbilityState(const UAbilityTask_StartAbilityState&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_StartAbilityState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_StartAbilityState(UAbilityTask_StartAbilityState&&); \
	NO_API UAbilityTask_StartAbilityState(const UAbilityTask_StartAbilityState&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_StartAbilityState); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_StartAbilityState); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_StartAbilityState)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_26_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h_29_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_StartAbilityState."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_StartAbilityState>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_StartAbilityState_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
