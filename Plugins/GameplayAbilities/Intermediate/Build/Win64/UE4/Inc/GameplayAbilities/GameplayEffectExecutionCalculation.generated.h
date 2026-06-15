// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayEffectCustomExecutionParameters;
struct FGameplayEffectCustomExecutionOutput;
#ifdef GAMEPLAYABILITIES_GameplayEffectExecutionCalculation_generated_h
#error "GameplayEffectExecutionCalculation.generated.h already included, missing '#pragma once' in GameplayEffectExecutionCalculation.h"
#endif
#define GAMEPLAYABILITIES_GameplayEffectExecutionCalculation_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_143_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics; \
	static class UScriptStruct* StaticStruct(); \
	FORCEINLINE static uint32 __PPO__OutputModifiers() { return STRUCT_OFFSET(FGameplayEffectCustomExecutionOutput, OutputModifiers); }


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayEffectCustomExecutionOutput>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_21_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics; \
	static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayEffectCustomExecutionParameters>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_RPC_WRAPPERS \
	virtual void Execute_Implementation(FGameplayEffectCustomExecutionParameters const& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const; \
 \
	DECLARE_FUNCTION(execExecute) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectCustomExecutionParameters,Z_Param_Out_ExecutionParams); \
		P_GET_STRUCT_REF(FGameplayEffectCustomExecutionOutput,Z_Param_Out_OutExecutionOutput); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->Execute_Implementation(Z_Param_Out_ExecutionParams,Z_Param_Out_OutExecutionOutput); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execExecute) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectCustomExecutionParameters,Z_Param_Out_ExecutionParams); \
		P_GET_STRUCT_REF(FGameplayEffectCustomExecutionOutput,Z_Param_Out_OutExecutionOutput); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->Execute_Implementation(Z_Param_Out_ExecutionParams,Z_Param_Out_OutExecutionOutput); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_EVENT_PARMS \
	struct GameplayEffectExecutionCalculation_eventExecute_Parms \
	{ \
		FGameplayEffectCustomExecutionParameters ExecutionParams; \
		FGameplayEffectCustomExecutionOutput OutExecutionOutput; \
	};


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_CALLBACK_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayEffectExecutionCalculation(); \
	friend struct Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics; \
public: \
	DECLARE_CLASS(UGameplayEffectExecutionCalculation, UGameplayEffectCalculation, COMPILED_IN_FLAGS(CLASS_Abstract), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayEffectExecutionCalculation)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_INCLASS \
private: \
	static void StaticRegisterNativesUGameplayEffectExecutionCalculation(); \
	friend struct Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics; \
public: \
	DECLARE_CLASS(UGameplayEffectExecutionCalculation, UGameplayEffectCalculation, COMPILED_IN_FLAGS(CLASS_Abstract), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayEffectExecutionCalculation)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayEffectExecutionCalculation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayEffectExecutionCalculation) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayEffectExecutionCalculation); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayEffectExecutionCalculation); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayEffectExecutionCalculation(UGameplayEffectExecutionCalculation&&); \
	NO_API UGameplayEffectExecutionCalculation(const UGameplayEffectExecutionCalculation&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayEffectExecutionCalculation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayEffectExecutionCalculation(UGameplayEffectExecutionCalculation&&); \
	NO_API UGameplayEffectExecutionCalculation(const UGameplayEffectExecutionCalculation&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayEffectExecutionCalculation); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayEffectExecutionCalculation); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayEffectExecutionCalculation)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__bRequiresPassedInTags() { return STRUCT_OFFSET(UGameplayEffectExecutionCalculation, bRequiresPassedInTags); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_199_PROLOG \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_EVENT_PARMS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h_202_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class GameplayEffectExecutionCalculation."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UGameplayEffectExecutionCalculation>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectExecutionCalculation_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
