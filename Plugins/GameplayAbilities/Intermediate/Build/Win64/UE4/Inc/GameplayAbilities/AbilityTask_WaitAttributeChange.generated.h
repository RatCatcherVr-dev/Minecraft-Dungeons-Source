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
struct FGameplayTag;
class AActor;
class UAbilityTask_WaitAttributeChange;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitAttributeChange_generated_h
#error "AbilityTask_WaitAttributeChange.generated.h already included, missing '#pragma once' in AbilityTask_WaitAttributeChange.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitAttributeChange_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_29_DELEGATE \
static inline void FWaitAttributeChangeDelegate_DelegateWrapper(const FMulticastScriptDelegate& WaitAttributeChangeDelegate) \
{ \
	WaitAttributeChangeDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitForAttributeChangeWithComparison) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_InAttribute); \
		P_GET_STRUCT(FGameplayTag,Z_Param_InWithTag); \
		P_GET_STRUCT(FGameplayTag,Z_Param_InWithoutTag); \
		P_GET_PROPERTY(UByteProperty,Z_Param_InComparisonType); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_InComparisonValue); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChange**)Z_Param__Result=UAbilityTask_WaitAttributeChange::WaitForAttributeChangeWithComparison(Z_Param_OwningAbility,Z_Param_InAttribute,Z_Param_InWithTag,Z_Param_InWithoutTag,EWaitAttributeChangeComparison::Type(Z_Param_InComparisonType),Z_Param_InComparisonValue,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWaitForAttributeChange) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithSrcTag); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithoutSrcTag); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChange**)Z_Param__Result=UAbilityTask_WaitAttributeChange::WaitForAttributeChange(Z_Param_OwningAbility,Z_Param_Attribute,Z_Param_WithSrcTag,Z_Param_WithoutSrcTag,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitForAttributeChangeWithComparison) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_InAttribute); \
		P_GET_STRUCT(FGameplayTag,Z_Param_InWithTag); \
		P_GET_STRUCT(FGameplayTag,Z_Param_InWithoutTag); \
		P_GET_PROPERTY(UByteProperty,Z_Param_InComparisonType); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_InComparisonValue); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChange**)Z_Param__Result=UAbilityTask_WaitAttributeChange::WaitForAttributeChangeWithComparison(Z_Param_OwningAbility,Z_Param_InAttribute,Z_Param_InWithTag,Z_Param_InWithoutTag,EWaitAttributeChangeComparison::Type(Z_Param_InComparisonType),Z_Param_InComparisonValue,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWaitForAttributeChange) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithSrcTag); \
		P_GET_STRUCT(FGameplayTag,Z_Param_WithoutSrcTag); \
		P_GET_UBOOL(Z_Param_TriggerOnce); \
		P_GET_OBJECT(AActor,Z_Param_OptionalExternalOwner); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitAttributeChange**)Z_Param__Result=UAbilityTask_WaitAttributeChange::WaitForAttributeChange(Z_Param_OwningAbility,Z_Param_Attribute,Z_Param_WithSrcTag,Z_Param_WithoutSrcTag,Z_Param_TriggerOnce,Z_Param_OptionalExternalOwner); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAttributeChange(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAttributeChange_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAttributeChange, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAttributeChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitAttributeChange(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitAttributeChange_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitAttributeChange, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitAttributeChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAttributeChange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAttributeChange) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAttributeChange); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAttributeChange); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAttributeChange(UAbilityTask_WaitAttributeChange&&); \
	NO_API UAbilityTask_WaitAttributeChange(const UAbilityTask_WaitAttributeChange&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitAttributeChange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitAttributeChange(UAbilityTask_WaitAttributeChange&&); \
	NO_API UAbilityTask_WaitAttributeChange(const UAbilityTask_WaitAttributeChange&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitAttributeChange); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitAttributeChange); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitAttributeChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__ExternalOwner() { return STRUCT_OFFSET(UAbilityTask_WaitAttributeChange, ExternalOwner); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_34_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h_37_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitAttributeChange."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitAttributeChange>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitAttributeChange_h


#define FOREACH_ENUM_EWAITATTRIBUTECHANGECOMPARISON(op) \
	op(EWaitAttributeChangeComparison::None) \
	op(EWaitAttributeChangeComparison::GreaterThan) \
	op(EWaitAttributeChangeComparison::LessThan) \
	op(EWaitAttributeChangeComparison::GreaterThanOrEqualTo) \
	op(EWaitAttributeChangeComparison::LessThanOrEqualTo) \
	op(EWaitAttributeChangeComparison::NotEqualTo) \
	op(EWaitAttributeChangeComparison::ExactlyEqualTo) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
