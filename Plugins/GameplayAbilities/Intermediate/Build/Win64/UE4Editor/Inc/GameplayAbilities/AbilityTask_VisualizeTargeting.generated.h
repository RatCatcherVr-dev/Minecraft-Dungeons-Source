// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
class AGameplayAbilityTargetActor;
class UAbilityTask_VisualizeTargeting;
#ifdef GAMEPLAYABILITIES_AbilityTask_VisualizeTargeting_generated_h
#error "AbilityTask_VisualizeTargeting.generated.h already included, missing '#pragma once' in AbilityTask_VisualizeTargeting.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_VisualizeTargeting_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_12_DELEGATE \
static inline void FVisualizeTargetingDelegate_DelegateWrapper(const FMulticastScriptDelegate& VisualizeTargetingDelegate) \
{ \
	VisualizeTargetingDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execFinishSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(AGameplayAbilityTargetActor,Z_Param_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->FinishSpawningActor(Z_Param_OwningAbility,Z_Param_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBeginSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_GET_OBJECT_REF(AGameplayAbilityTargetActor,Z_Param_Out_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->BeginSpawningActor(Z_Param_OwningAbility,Z_Param_Class,Z_Param_Out_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execVisualizeTargetingUsingActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(AGameplayAbilityTargetActor,Z_Param_TargetActor); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_VisualizeTargeting**)Z_Param__Result=UAbilityTask_VisualizeTargeting::VisualizeTargetingUsingActor(Z_Param_OwningAbility,Z_Param_TargetActor,Z_Param_TaskInstanceName,Z_Param_Duration); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execVisualizeTargeting) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_VisualizeTargeting**)Z_Param__Result=UAbilityTask_VisualizeTargeting::VisualizeTargeting(Z_Param_OwningAbility,Z_Param_Class,Z_Param_TaskInstanceName,Z_Param_Duration); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execFinishSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(AGameplayAbilityTargetActor,Z_Param_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->FinishSpawningActor(Z_Param_OwningAbility,Z_Param_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBeginSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_GET_OBJECT_REF(AGameplayAbilityTargetActor,Z_Param_Out_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->BeginSpawningActor(Z_Param_OwningAbility,Z_Param_Class,Z_Param_Out_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execVisualizeTargetingUsingActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(AGameplayAbilityTargetActor,Z_Param_TargetActor); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_VisualizeTargeting**)Z_Param__Result=UAbilityTask_VisualizeTargeting::VisualizeTargetingUsingActor(Z_Param_OwningAbility,Z_Param_TargetActor,Z_Param_TaskInstanceName,Z_Param_Duration); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execVisualizeTargeting) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_VisualizeTargeting**)Z_Param__Result=UAbilityTask_VisualizeTargeting::VisualizeTargeting(Z_Param_OwningAbility,Z_Param_Class,Z_Param_TaskInstanceName,Z_Param_Duration); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_VisualizeTargeting(); \
	friend struct Z_Construct_UClass_UAbilityTask_VisualizeTargeting_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_VisualizeTargeting, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_VisualizeTargeting)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_VisualizeTargeting(); \
	friend struct Z_Construct_UClass_UAbilityTask_VisualizeTargeting_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_VisualizeTargeting, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_VisualizeTargeting)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_VisualizeTargeting(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_VisualizeTargeting) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_VisualizeTargeting); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_VisualizeTargeting); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_VisualizeTargeting(UAbilityTask_VisualizeTargeting&&); \
	NO_API UAbilityTask_VisualizeTargeting(const UAbilityTask_VisualizeTargeting&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_VisualizeTargeting(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_VisualizeTargeting(UAbilityTask_VisualizeTargeting&&); \
	NO_API UAbilityTask_VisualizeTargeting(const UAbilityTask_VisualizeTargeting&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_VisualizeTargeting); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_VisualizeTargeting); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_VisualizeTargeting)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_14_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h_17_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_VisualizeTargeting."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_VisualizeTargeting>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_VisualizeTargeting_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
