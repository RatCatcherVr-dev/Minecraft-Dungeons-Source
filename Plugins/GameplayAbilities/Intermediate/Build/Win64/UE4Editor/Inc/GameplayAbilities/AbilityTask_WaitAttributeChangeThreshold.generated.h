// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
struct FGameplayAttribute;
class AActor;
class UAbilityTask_WaitAttributeChangeThreshold;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitAttributeChangeThreshold_generated_h
#error "AbilityTask_WaitAttributeChangeThreshold.generated.h already included, missing '#pragma once' in AbilityTask_WaitAttributeChangeThreshold.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitAttributeChangeThreshold_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_13_DELEGATE \
struct _Script_GameplayAbilities_eventWaitAttributeChangeThresholdDelegate_Parms \
{ \
	bool bMatchesComparison; \
	float CurrentValue; \
}; \
static inline void FWaitAttributeChangeThresholdDelegate_DelegateWrapper(const FMulticastScriptDelegate& WaitAttributeChangeThresholdDelegate, bool bMatchesComparison, float CurrentValue) \
{ \
	_Script_GameplayAbilities_eventWaitAttributeChangeThresholdDelegate_Parms Parms; \
	Parms.bMatchesComparison=bMatchesComparison ? true : false; \
	Parms.CurrentValue=CurrentValue; \
	WaitAttributeChangeThresholdDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitForAttributeChangeThreshold) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_PROPERTY(UByteProperty,Z_Param_ComparisonType); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ComparisonValue); \
		P_GET_UBOOL(Z_Param_bTriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChangeThreshold**)Z_Param__Result=UAbilityTask_WaitAttributeChangeThreshold::WaitForAttributeChangeThreshold(Z_Param_OwningAbility,Z_Param_Attribute,EWaitAttributeChangeComparison::Type(Z_Param_ComparisonType),Z_Param_ComparisonValue,Z_Param_bTriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitForAttributeChangeThreshold) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_PROPERTY(UByteProperty,Z_Param_ComparisonType); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ComparisonValue); \
		P_GET_UBOOL(Z_Param_bTriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChangeThreshold**)Z_Param__Result=UAbilityTask_WaitAttributeChangeThreshold::WaitForAttributeChangeThreshold(Z_Param_OwningAbility,Z_Param_Attribute,EWaitAttributeChangeComparison::Type(Z_Param_ComparisonType),Z_Param_ComparisonValue,Z_Param_bTriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAttributeChangeThreshold(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAttributeChangeThreshold_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAttributeChangeThreshold, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAttributeChangeThreshold)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAttributeChangeThreshold(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAttributeChangeThreshold_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAttributeChangeThreshold, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAttributeChangeThreshold)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAttributeChangeThreshold(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAttributeChangeThreshold) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAttributeChangeThreshold); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAttributeChangeThreshold); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAttributeChangeThreshold(UAbilityTask_WaitAttributeChangeThreshold&&); \
	NO_API UAbilityTask_WaitAttributeChangeThreshold(const UAbilityTask_WaitAttributeChangeThreshold&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAttributeChangeThreshold(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAttributeChangeThreshold(UAbilityTask_WaitAttributeChangeThreshold&&); \
	NO_API UAbilityTask_WaitAttributeChangeThreshold(const UAbilityTask_WaitAttributeChangeThreshold&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAttributeChangeThreshold); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAttributeChangeThreshold); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAttributeChangeThreshold)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__ExternalOwner() { return STRUCT_OFFSET(UAbilityTask_WaitAttributeChangeThreshold, ExternalOwner); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_18_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h_21_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitAttributeChangeThreshold."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitAttributeChangeThreshold>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeThreshold_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
