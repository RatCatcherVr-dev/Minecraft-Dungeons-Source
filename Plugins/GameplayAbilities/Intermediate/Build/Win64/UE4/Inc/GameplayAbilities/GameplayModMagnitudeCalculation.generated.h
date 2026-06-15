// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayEffectSpec;
#ifdef GAMEPLAYABILITIES_GameplayModMagnitudeCalculation_generated_h
#error "GameplayModMagnitudeCalculation.generated.h already included, missing '#pragma once' in GameplayModMagnitudeCalculation.h"
#endif
#define GAMEPLAYABILITIES_GameplayModMagnitudeCalculation_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_RPC_WRAPPERS \
	virtual float CalculateBaseMagnitude_Implementation(FGameplayEffectSpec const& Spec) const; \
 \
	DECLARE_FUNCTION(execCalculateBaseMagnitude) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectSpec,Z_Param_Out_Spec); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->CalculateBaseMagnitude_Implementation(Z_Param_Out_Spec); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual float CalculateBaseMagnitude_Implementation(FGameplayEffectSpec const& Spec) const; \
 \
	DECLARE_FUNCTION(execCalculateBaseMagnitude) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectSpec,Z_Param_Out_Spec); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->CalculateBaseMagnitude_Implementation(Z_Param_Out_Spec); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_EVENT_PARMS \
	struct GameplayModMagnitudeCalculation_eventCalculateBaseMagnitude_Parms \
	{ \
		FGameplayEffectSpec Spec; \
		float ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayModMagnitudeCalculation_eventCalculateBaseMagnitude_Parms() \
			: ReturnValue(0) \
		{ \
		} \
	};


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_CALLBACK_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayModMagnitudeCalculation(); \
	friend struct Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics; \
public: \
	DECLARE_CLASS(UGameplayModMagnitudeCalculation, UGameplayEffectCalculation, COMPILED_IN_FLAGS(CLASS_Abstract), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayModMagnitudeCalculation)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_INCLASS \
private: \
	static void StaticRegisterNativesUGameplayModMagnitudeCalculation(); \
	friend struct Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics; \
public: \
	DECLARE_CLASS(UGameplayModMagnitudeCalculation, UGameplayEffectCalculation, COMPILED_IN_FLAGS(CLASS_Abstract), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayModMagnitudeCalculation)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayModMagnitudeCalculation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayModMagnitudeCalculation) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayModMagnitudeCalculation); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayModMagnitudeCalculation); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayModMagnitudeCalculation(UGameplayModMagnitudeCalculation&&); \
	NO_API UGameplayModMagnitudeCalculation(const UGameplayModMagnitudeCalculation&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayModMagnitudeCalculation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayModMagnitudeCalculation(UGameplayModMagnitudeCalculation&&); \
	NO_API UGameplayModMagnitudeCalculation(const UGameplayModMagnitudeCalculation&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayModMagnitudeCalculation); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayModMagnitudeCalculation); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayModMagnitudeCalculation)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__bAllowNonNetAuthorityDependencyRegistration() { return STRUCT_OFFSET(UGameplayModMagnitudeCalculation, bAllowNonNetAuthorityDependencyRegistration); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_12_PROLOG \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_EVENT_PARMS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h_17_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class GameplayModMagnitudeCalculation."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UGameplayModMagnitudeCalculation>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayModMagnitudeCalculation_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
