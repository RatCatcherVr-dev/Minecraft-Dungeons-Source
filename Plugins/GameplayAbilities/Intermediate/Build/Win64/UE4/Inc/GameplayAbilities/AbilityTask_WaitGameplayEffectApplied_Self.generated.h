// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
struct FGameplayEffectSpecHandle;
struct FActiveGameplayEffectHandle;
class UGameplayAbility;
struct FGameplayTargetDataFilterHandle;
struct FGameplayTagQuery;
class UAbilityTask_WaitGameplayEffectApplied_Self;
struct FGameplayTagRequirements;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitGameplayEffectApplied_Self_generated_h
#error "AbilityTask_WaitGameplayEffectApplied_Self.generated.h already included, missing '#pragma once' in AbilityTask_WaitGameplayEffectApplied_Self.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitGameplayEffectApplied_Self_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_11_DELEGATE \
struct _Script_GameplayAbilities_eventGameplayEffectAppliedSelfDelegate_Parms \
{ \
	AActor* Source; \
	FGameplayEffectSpecHandle SpecHandle; \
	FActiveGameplayEffectHandle ActiveHandle; \
}; \
static inline void FGameplayEffectAppliedSelfDelegate_DelegateWrapper(const FMulticastScriptDelegate& GameplayEffectAppliedSelfDelegate, AActor* Source, FGameplayEffectSpecHandle SpecHandle, FActiveGameplayEffectHandle ActiveHandle) \
{ \
	_Script_GameplayAbilities_eventGameplayEffectAppliedSelfDelegate_Parms Parms; \
	Parms.Source=Source; \
	Parms.SpecHandle=SpecHandle; \
	Parms.ActiveHandle=ActiveHandle; \
	GameplayEffectAppliedSelfDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitGameplayEffectAppliedToSelf_Query) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTargetDataFilterHandle,Z_Param_SourceFilter); \
		P_GET_STRUCT(FGameplayTagQuery,Z_Param_SourceTagQuery); \
		P_GET_STRUCT(FGameplayTagQuery,Z_Param_TargetTagQuery); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_GET_UBOOL(Z_Param_ListenForPeriodicEffect); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitGameplayEffectApplied_Self**)Z_Param__Result=UAbilityTask_WaitGameplayEffectApplied_Self::WaitGameplayEffectAppliedToSelf_Query(Z_Param_OwningAbility,Z_Param_SourceFilter,Z_Param_SourceTagQuery,Z_Param_TargetTagQuery,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner,Z_Param_ListenForPeriodicEffect); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWaitGameplayEffectAppliedToSelf) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTargetDataFilterHandle,Z_Param_SourceFilter); \
		P_GET_STRUCT(FGameplayTagRequirements,Z_Param_SourceTagRequirements); \
		P_GET_STRUCT(FGameplayTagRequirements,Z_Param_TargetTagRequirements); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_GET_UBOOL(Z_Param_ListenForPeriodicEffect); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitGameplayEffectApplied_Self**)Z_Param__Result=UAbilityTask_WaitGameplayEffectApplied_Self::WaitGameplayEffectAppliedToSelf(Z_Param_OwningAbility,Z_Param_SourceFilter,Z_Param_SourceTagRequirements,Z_Param_TargetTagRequirements,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner,Z_Param_ListenForPeriodicEffect); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitGameplayEffectAppliedToSelf_Query) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTargetDataFilterHandle,Z_Param_SourceFilter); \
		P_GET_STRUCT(FGameplayTagQuery,Z_Param_SourceTagQuery); \
		P_GET_STRUCT(FGameplayTagQuery,Z_Param_TargetTagQuery); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_GET_UBOOL(Z_Param_ListenForPeriodicEffect); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitGameplayEffectApplied_Self**)Z_Param__Result=UAbilityTask_WaitGameplayEffectApplied_Self::WaitGameplayEffectAppliedToSelf_Query(Z_Param_OwningAbility,Z_Param_SourceFilter,Z_Param_SourceTagQuery,Z_Param_TargetTagQuery,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner,Z_Param_ListenForPeriodicEffect); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWaitGameplayEffectAppliedToSelf) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayTargetDataFilterHandle,Z_Param_SourceFilter); \
		P_GET_STRUCT(FGameplayTagRequirements,Z_Param_SourceTagRequirements); \
		P_GET_STRUCT(FGameplayTagRequirements,Z_Param_TargetTagRequirements); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_GET_UBOOL(Z_Param_ListenForPeriodicEffect); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitGameplayEffectApplied_Self**)Z_Param__Result=UAbilityTask_WaitGameplayEffectApplied_Self::WaitGameplayEffectAppliedToSelf(Z_Param_OwningAbility,Z_Param_SourceFilter,Z_Param_SourceTagRequirements,Z_Param_TargetTagRequirements,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner,Z_Param_ListenForPeriodicEffect); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitGameplayEffectApplied_Self(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitGameplayEffectApplied_Self_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitGameplayEffectApplied_Self, UAbilityTask_WaitGameplayEffectApplied, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitGameplayEffectApplied_Self)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitGameplayEffectApplied_Self(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitGameplayEffectApplied_Self_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitGameplayEffectApplied_Self, UAbilityTask_WaitGameplayEffectApplied, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitGameplayEffectApplied_Self)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitGameplayEffectApplied_Self(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitGameplayEffectApplied_Self) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitGameplayEffectApplied_Self); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitGameplayEffectApplied_Self); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitGameplayEffectApplied_Self(UAbilityTask_WaitGameplayEffectApplied_Self&&); \
	NO_API UAbilityTask_WaitGameplayEffectApplied_Self(const UAbilityTask_WaitGameplayEffectApplied_Self&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitGameplayEffectApplied_Self(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitGameplayEffectApplied_Self(UAbilityTask_WaitGameplayEffectApplied_Self&&); \
	NO_API UAbilityTask_WaitGameplayEffectApplied_Self(const UAbilityTask_WaitGameplayEffectApplied_Self&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitGameplayEffectApplied_Self); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitGameplayEffectApplied_Self); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitGameplayEffectApplied_Self)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_13_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h_16_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitGameplayEffectApplied_Self."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitGameplayEffectApplied_Self>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitGameplayEffectApplied_Self_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
