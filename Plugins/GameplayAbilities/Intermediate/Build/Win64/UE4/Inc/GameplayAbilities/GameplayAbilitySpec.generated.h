// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_GameplayAbilitySpec_generated_h
#error "GameplayAbilitySpec.generated.h already included, missing '#pragma once' in GameplayAbilitySpec.h"
#endif
#define GAMEPLAYABILITIES_GameplayAbilitySpec_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySpec_h_316_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics; \
	static class UScriptStruct* StaticStruct(); \
	typedef FFastArraySerializer Super;


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilitySpecContainer>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySpec_h_218_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics; \
	static class UScriptStruct* StaticStruct(); \
	typedef FFastArraySerializerItem Super;


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilitySpec>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySpec_h_158_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics; \
	static class UScriptStruct* StaticStruct(); \
	FORCEINLINE static uint32 __PPO__PredictionKeyWhenActivated() { return STRUCT_OFFSET(FGameplayAbilityActivationInfo, PredictionKeyWhenActivated); }


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityActivationInfo>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySpec_h_116_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilitySpecDef>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySpec_h_27_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct(); \
	FORCEINLINE static uint32 __PPO__Handle() { return STRUCT_OFFSET(FGameplayAbilitySpecHandle, Handle); }


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilitySpecHandle>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySpec_h


#define FOREACH_ENUM_EGAMEPLAYEFFECTGRANTEDABILITYREMOVEPOLICY(op) \
	op(EGameplayEffectGrantedAbilityRemovePolicy::CancelAbilityImmediately) \
	op(EGameplayEffectGrantedAbilityRemovePolicy::RemoveAbilityOnEnd) \
	op(EGameplayEffectGrantedAbilityRemovePolicy::DoNothing) 

enum class EGameplayEffectGrantedAbilityRemovePolicy : uint8;
template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EGameplayEffectGrantedAbilityRemovePolicy>();

#define FOREACH_ENUM_EGAMEPLAYABILITYACTIVATIONMODE(op) \
	op(EGameplayAbilityActivationMode::Authority) \
	op(EGameplayAbilityActivationMode::NonAuthority) \
	op(EGameplayAbilityActivationMode::Predicting) \
	op(EGameplayAbilityActivationMode::Confirmed) \
	op(EGameplayAbilityActivationMode::Rejected) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
