// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_GameplayAbilitySet_generated_h
#error "GameplayAbilitySet.generated.h already included, missing '#pragma once' in GameplayAbilitySet.h"
#endif
#define GAMEPLAYABILITIES_GameplayAbilitySet_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_45_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayAbilityBindInfo>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_RPC_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_RPC_WRAPPERS_NO_PURE_DECLS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayAbilitySet(); \
	friend struct Z_Construct_UClass_UGameplayAbilitySet_Statics; \
public: \
	DECLARE_CLASS(UGameplayAbilitySet, UDataAsset, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayAbilitySet)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_INCLASS \
private: \
	static void StaticRegisterNativesUGameplayAbilitySet(); \
	friend struct Z_Construct_UClass_UGameplayAbilitySet_Statics; \
public: \
	DECLARE_CLASS(UGameplayAbilitySet, UDataAsset, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayAbilitySet)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayAbilitySet) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayAbilitySet); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayAbilitySet); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayAbilitySet(UGameplayAbilitySet&&); \
	NO_API UGameplayAbilitySet(const UGameplayAbilitySet&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayAbilitySet(UGameplayAbilitySet&&); \
	NO_API UGameplayAbilitySet(const UGameplayAbilitySet&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayAbilitySet); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayAbilitySet); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayAbilitySet)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_60_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h_63_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class GameplayAbilitySet."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UGameplayAbilitySet>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayAbilitySet_h


#define FOREACH_ENUM_EGAMEPLAYABILITYINPUTBINDS(op) \
	op(EGameplayAbilityInputBinds::Ability1) \
	op(EGameplayAbilityInputBinds::Ability2) \
	op(EGameplayAbilityInputBinds::Ability3) \
	op(EGameplayAbilityInputBinds::Ability4) \
	op(EGameplayAbilityInputBinds::Ability5) \
	op(EGameplayAbilityInputBinds::Ability6) \
	op(EGameplayAbilityInputBinds::Ability7) \
	op(EGameplayAbilityInputBinds::Ability8) \
	op(EGameplayAbilityInputBinds::Ability9) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
