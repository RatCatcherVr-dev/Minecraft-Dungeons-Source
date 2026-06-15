// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
struct FVector;
class UAbilityTask_WaitVelocityChange;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitVelocityChange_generated_h
#error "AbilityTask_WaitVelocityChange.generated.h already included, missing '#pragma once' in AbilityTask_WaitVelocityChange.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitVelocityChange_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_11_DELEGATE \
static inline void FWaitVelocityChangeDelegate_DelegateWrapper(const FMulticastScriptDelegate& WaitVelocityChangeDelegate) \
{ \
	WaitVelocityChangeDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execCreateWaitVelocityChange) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FVector,Z_Param_Direction); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_MinimumMagnitude); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitVelocityChange**)Z_Param__Result=UAbilityTask_WaitVelocityChange::CreateWaitVelocityChange(Z_Param_OwningAbility,Z_Param_Direction,Z_Param_MinimumMagnitude); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execCreateWaitVelocityChange) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FVector,Z_Param_Direction); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_MinimumMagnitude); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitVelocityChange**)Z_Param__Result=UAbilityTask_WaitVelocityChange::CreateWaitVelocityChange(Z_Param_OwningAbility,Z_Param_Direction,Z_Param_MinimumMagnitude); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitVelocityChange(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitVelocityChange_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitVelocityChange, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitVelocityChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitVelocityChange(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitVelocityChange_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitVelocityChange, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitVelocityChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitVelocityChange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitVelocityChange) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitVelocityChange); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitVelocityChange); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitVelocityChange(UAbilityTask_WaitVelocityChange&&); \
	NO_API UAbilityTask_WaitVelocityChange(const UAbilityTask_WaitVelocityChange&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitVelocityChange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitVelocityChange(UAbilityTask_WaitVelocityChange&&); \
	NO_API UAbilityTask_WaitVelocityChange(const UAbilityTask_WaitVelocityChange&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitVelocityChange); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitVelocityChange); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitVelocityChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__CachedMovementComponent() { return STRUCT_OFFSET(UAbilityTask_WaitVelocityChange, CachedMovementComponent); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_13_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h_16_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitVelocityChange."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitVelocityChange>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitVelocityChange_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
