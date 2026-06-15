// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef GAMEPLAYABILITIES_AbilitySystemTestPawn_generated_h
#error "AbilitySystemTestPawn.generated.h already included, missing '#pragma once' in AbilitySystemTestPawn.h"
#endif
#define GAMEPLAYABILITIES_AbilitySystemTestPawn_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_RPC_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_RPC_WRAPPERS_NO_PURE_DECLS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAAbilitySystemTestPawn(); \
	friend struct Z_Construct_UClass_AAbilitySystemTestPawn_Statics; \
public: \
	DECLARE_CLASS(AAbilitySystemTestPawn, ADefaultPawn, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(AAbilitySystemTestPawn) \
	virtual UObject* _getUObject() const override { return const_cast<AAbilitySystemTestPawn*>(this); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_INCLASS \
private: \
	static void StaticRegisterNativesAAbilitySystemTestPawn(); \
	friend struct Z_Construct_UClass_AAbilitySystemTestPawn_Statics; \
public: \
	DECLARE_CLASS(AAbilitySystemTestPawn, ADefaultPawn, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(AAbilitySystemTestPawn) \
	virtual UObject* _getUObject() const override { return const_cast<AAbilitySystemTestPawn*>(this); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AAbilitySystemTestPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AAbilitySystemTestPawn) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AAbilitySystemTestPawn); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AAbilitySystemTestPawn); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AAbilitySystemTestPawn(AAbilitySystemTestPawn&&); \
	NO_API AAbilitySystemTestPawn(const AAbilitySystemTestPawn&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AAbilitySystemTestPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AAbilitySystemTestPawn(AAbilitySystemTestPawn&&); \
	NO_API AAbilitySystemTestPawn(const AAbilitySystemTestPawn&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AAbilitySystemTestPawn); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AAbilitySystemTestPawn); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AAbilitySystemTestPawn)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__AbilitySystemComponent() { return STRUCT_OFFSET(AAbilitySystemTestPawn, AbilitySystemComponent); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_14_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h_17_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilitySystemTestPawn."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class AAbilitySystemTestPawn>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemTestPawn_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
