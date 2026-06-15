// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
class UGameplayAbility;
struct FGameplayAbilityTargetDataHandle;
class UAbilityTask_SpawnActor;
#ifdef GAMEPLAYABILITIES_AbilityTask_SpawnActor_generated_h
#error "AbilityTask_SpawnActor.generated.h already included, missing '#pragma once' in AbilityTask_SpawnActor.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_SpawnActor_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_12_DELEGATE \
struct _Script_GameplayAbilities_eventSpawnActorDelegate_Parms \
{ \
	AActor* SpawnedActor; \
}; \
static inline void FSpawnActorDelegate_DelegateWrapper(const FMulticastScriptDelegate& SpawnActorDelegate, AActor* SpawnedActor) \
{ \
	_Script_GameplayAbilities_eventSpawnActorDelegate_Parms Parms; \
	Parms.SpawnedActor=SpawnedActor; \
	SpawnActorDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execFinishSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(AActor,Z_Param_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->FinishSpawningActor(Z_Param_OwningAbility,Z_Param_TargetData,Z_Param_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBeginSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_GET_OBJECT_REF(AActor,Z_Param_Out_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->BeginSpawningActor(Z_Param_OwningAbility,Z_Param_TargetData,Z_Param_Class,Z_Param_Out_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSpawnActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_SpawnActor**)Z_Param__Result=UAbilityTask_SpawnActor::SpawnActor(Z_Param_OwningAbility,Z_Param_TargetData,Z_Param_Class); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execFinishSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(AActor,Z_Param_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->FinishSpawningActor(Z_Param_OwningAbility,Z_Param_TargetData,Z_Param_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBeginSpawningActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_GET_OBJECT_REF(AActor,Z_Param_Out_SpawnedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->BeginSpawningActor(Z_Param_OwningAbility,Z_Param_TargetData,Z_Param_Class,Z_Param_Out_SpawnedActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSpawnActor) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(UClass,Z_Param_Class); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_SpawnActor**)Z_Param__Result=UAbilityTask_SpawnActor::SpawnActor(Z_Param_OwningAbility,Z_Param_TargetData,Z_Param_Class); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_SpawnActor(); \
	friend struct Z_Construct_UClass_UAbilityTask_SpawnActor_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_SpawnActor, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_SpawnActor)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_SpawnActor(); \
	friend struct Z_Construct_UClass_UAbilityTask_SpawnActor_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_SpawnActor, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_SpawnActor)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_SpawnActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_SpawnActor) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_SpawnActor); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_SpawnActor); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_SpawnActor(UAbilityTask_SpawnActor&&); \
	NO_API UAbilityTask_SpawnActor(const UAbilityTask_SpawnActor&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_SpawnActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_SpawnActor(UAbilityTask_SpawnActor&&); \
	NO_API UAbilityTask_SpawnActor(const UAbilityTask_SpawnActor&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_SpawnActor); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_SpawnActor); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_SpawnActor)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_26_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h_29_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_SpawnActor."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_SpawnActor>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_SpawnActor_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
