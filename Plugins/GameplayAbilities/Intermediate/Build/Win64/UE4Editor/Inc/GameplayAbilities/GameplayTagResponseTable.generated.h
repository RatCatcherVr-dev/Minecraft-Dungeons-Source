// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayTag;
class UAbilitySystemComponent;
#ifdef GAMEPLAYABILITIES_GameplayTagResponseTable_generated_h
#error "GameplayTagResponseTable.generated.h already included, missing '#pragma once' in GameplayTagResponseTable.h"
#endif
#define GAMEPLAYABILITIES_GameplayTagResponseTable_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_41_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayTagResponseTableEntry>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_19_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FGameplayTagReponsePair>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execTagResponseEvent) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_Tag); \
		P_GET_PROPERTY(UIntProperty,Z_Param_NewCount); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_ASC); \
		P_GET_PROPERTY(UIntProperty,Z_Param_idx); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->TagResponseEvent(Z_Param_Tag,Z_Param_NewCount,Z_Param_ASC,Z_Param_idx); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execTagResponseEvent) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_Tag); \
		P_GET_PROPERTY(UIntProperty,Z_Param_NewCount); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_ASC); \
		P_GET_PROPERTY(UIntProperty,Z_Param_idx); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->TagResponseEvent(Z_Param_Tag,Z_Param_NewCount,Z_Param_ASC,Z_Param_idx); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayTagReponseTable(); \
	friend struct Z_Construct_UClass_UGameplayTagReponseTable_Statics; \
public: \
	DECLARE_CLASS(UGameplayTagReponseTable, UDataAsset, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayTagReponseTable)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_INCLASS \
private: \
	static void StaticRegisterNativesUGameplayTagReponseTable(); \
	friend struct Z_Construct_UClass_UGameplayTagReponseTable_Statics; \
public: \
	DECLARE_CLASS(UGameplayTagReponseTable, UDataAsset, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayTagReponseTable)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayTagReponseTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayTagReponseTable) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayTagReponseTable); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayTagReponseTable); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayTagReponseTable(UGameplayTagReponseTable&&); \
	NO_API UGameplayTagReponseTable(const UGameplayTagReponseTable&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayTagReponseTable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayTagReponseTable(UGameplayTagReponseTable&&); \
	NO_API UGameplayTagReponseTable(const UGameplayTagReponseTable&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayTagReponseTable); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayTagReponseTable); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayTagReponseTable)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_59_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h_62_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class GameplayTagReponseTable."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UGameplayTagReponseTable>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayTagResponseTable_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
