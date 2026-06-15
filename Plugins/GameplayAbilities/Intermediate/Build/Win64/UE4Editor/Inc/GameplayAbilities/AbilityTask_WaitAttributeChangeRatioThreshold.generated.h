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
class UAbilityTask_WaitAttributeChangeRatioThreshold;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitAttributeChangeRatioThreshold_generated_h
#error "AbilityTask_WaitAttributeChangeRatioThreshold.generated.h already included, missing '#pragma once' in AbilityTask_WaitAttributeChangeRatioThreshold.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitAttributeChangeRatioThreshold_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_14_DELEGATE \
struct _Script_GameplayAbilities_eventWaitAttributeChangeRatioThresholdDelegate_Parms \
{ \
	bool bMatchesComparison; \
	float CurrentRatio; \
}; \
static inline void FWaitAttributeChangeRatioThresholdDelegate_DelegateWrapper(const FMulticastScriptDelegate& WaitAttributeChangeRatioThresholdDelegate, bool bMatchesComparison, float CurrentRatio) \
{ \
	_Script_GameplayAbilities_eventWaitAttributeChangeRatioThresholdDelegate_Parms Parms; \
	Parms.bMatchesComparison=bMatchesComparison ? true : false; \
	Parms.CurrentRatio=CurrentRatio; \
	WaitAttributeChangeRatioThresholdDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitForAttributeChangeRatioThreshold) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeNumerator); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeDenominator); \
		P_GET_PROPERTY(UByteProperty,Z_Param_ComparisonType); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ComparisonValue); \
		P_GET_UBOOL(Z_Param_bTriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChangeRatioThreshold**)Z_Param__Result=UAbilityTask_WaitAttributeChangeRatioThreshold::WaitForAttributeChangeRatioThreshold(Z_Param_OwningAbility,Z_Param_AttributeNumerator,Z_Param_AttributeDenominator,EWaitAttributeChangeComparison::Type(Z_Param_ComparisonType),Z_Param_ComparisonValue,Z_Param_bTriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitForAttributeChangeRatioThreshold) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeNumerator); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeDenominator); \
		P_GET_PROPERTY(UByteProperty,Z_Param_ComparisonType); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ComparisonValue); \
		P_GET_UBOOL(Z_Param_bTriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChangeRatioThreshold**)Z_Param__Result=UAbilityTask_WaitAttributeChangeRatioThreshold::WaitForAttributeChangeRatioThreshold(Z_Param_OwningAbility,Z_Param_AttributeNumerator,Z_Param_AttributeDenominator,EWaitAttributeChangeComparison::Type(Z_Param_ComparisonType),Z_Param_ComparisonValue,Z_Param_bTriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAttributeChangeRatioThreshold(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAttributeChangeRatioThreshold_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAttributeChangeRatioThreshold, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAttributeChangeRatioThreshold)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAttributeChangeRatioThreshold(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAttributeChangeRatioThreshold_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAttributeChangeRatioThreshold, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAttributeChangeRatioThreshold)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAttributeChangeRatioThreshold(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAttributeChangeRatioThreshold) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAttributeChangeRatioThreshold); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAttributeChangeRatioThreshold); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAttributeChangeRatioThreshold(UAbilityTask_WaitAttributeChangeRatioThreshold&&); \
	NO_API UAbilityTask_WaitAttributeChangeRatioThreshold(const UAbilityTask_WaitAttributeChangeRatioThreshold&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAttributeChangeRatioThreshold(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAttributeChangeRatioThreshold(UAbilityTask_WaitAttributeChangeRatioThreshold&&); \
	NO_API UAbilityTask_WaitAttributeChangeRatioThreshold(const UAbilityTask_WaitAttributeChangeRatioThreshold&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAttributeChangeRatioThreshold); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAttributeChangeRatioThreshold); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAttributeChangeRatioThreshold)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__ExternalOwner() { return STRUCT_OFFSET(UAbilityTask_WaitAttributeChangeRatioThreshold, ExternalOwner); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_19_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h_22_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitAttributeChangeRatioThreshold."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitAttributeChangeRatioThreshold>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChangeRatioThreshold_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
