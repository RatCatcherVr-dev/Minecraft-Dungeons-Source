// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
class UAbilityTask_WaitMovementModeChange;
class ACharacter;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitMovementModeChange_generated_h
#error "AbilityTask_WaitMovementModeChange.generated.h already included, missing '#pragma once' in AbilityTask_WaitMovementModeChange.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitMovementModeChange_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_11_DELEGATE \
struct _Script_GameplayAbilities_eventMovementModeChangedDelegate_Parms \
{ \
	TEnumAsByte<EMovementMode> NewMovementMode; \
}; \
static inline void FMovementModeChangedDelegate_DelegateWrapper(const FMulticastScriptDelegate& MovementModeChangedDelegate, EMovementMode NewMovementMode) \
{ \
	_Script_GameplayAbilities_eventMovementModeChangedDelegate_Parms Parms; \
	Parms.NewMovementMode=NewMovementMode; \
	MovementModeChangedDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execCreateWaitMovementModeChange) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UByteProperty,Z_Param_NewMode); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitMovementModeChange**)Z_Param__Result=UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(Z_Param_OwningAbility,EMovementMode(Z_Param_NewMode)); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnMovementModeChange) \
	{ \
		P_GET_OBJECT(ACharacter,Z_Param_Character); \
		P_GET_PROPERTY(UByteProperty,Z_Param_PrevMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_PreviousCustomMode); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnMovementModeChange(Z_Param_Character,EMovementMode(Z_Param_PrevMovementMode),Z_Param_PreviousCustomMode); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execCreateWaitMovementModeChange) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UByteProperty,Z_Param_NewMode); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitMovementModeChange**)Z_Param__Result=UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(Z_Param_OwningAbility,EMovementMode(Z_Param_NewMode)); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnMovementModeChange) \
	{ \
		P_GET_OBJECT(ACharacter,Z_Param_Character); \
		P_GET_PROPERTY(UByteProperty,Z_Param_PrevMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_PreviousCustomMode); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnMovementModeChange(Z_Param_Character,EMovementMode(Z_Param_PrevMovementMode),Z_Param_PreviousCustomMode); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitMovementModeChange(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitMovementModeChange_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitMovementModeChange, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitMovementModeChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitMovementModeChange(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitMovementModeChange_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitMovementModeChange, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitMovementModeChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitMovementModeChange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitMovementModeChange) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitMovementModeChange); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitMovementModeChange); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitMovementModeChange(UAbilityTask_WaitMovementModeChange&&); \
	NO_API UAbilityTask_WaitMovementModeChange(const UAbilityTask_WaitMovementModeChange&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitMovementModeChange(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitMovementModeChange(UAbilityTask_WaitMovementModeChange&&); \
	NO_API UAbilityTask_WaitMovementModeChange(const UAbilityTask_WaitMovementModeChange&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitMovementModeChange); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitMovementModeChange); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitMovementModeChange)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_15_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h_18_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitMovementModeChange."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitMovementModeChange>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitMovementModeChange_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
