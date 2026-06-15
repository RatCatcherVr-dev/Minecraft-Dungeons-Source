// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayAbilityTargetDataHandle;
class UGameplayAbility;
class UAbilityTask_WaitOverlap;
class UPrimitiveComponent;
class AActor;
struct FVector;
struct FHitResult;
#ifdef GAMEPLAYABILITIES_AbilityTask_WaitOverlap_generated_h
#error "AbilityTask_WaitOverlap.generated.h already included, missing '#pragma once' in AbilityTask_WaitOverlap.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_WaitOverlap_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_9_DELEGATE \
struct _Script_GameplayAbilities_eventWaitOverlapDelegate_Parms \
{ \
	FGameplayAbilityTargetDataHandle TargetData; \
}; \
static inline void FWaitOverlapDelegate_DelegateWrapper(const FMulticastScriptDelegate& WaitOverlapDelegate, FGameplayAbilityTargetDataHandle const& TargetData) \
{ \
	_Script_GameplayAbilities_eventWaitOverlapDelegate_Parms Parms; \
	Parms.TargetData=TargetData; \
	WaitOverlapDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execWaitForOverlap) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitOverlap**)Z_Param__Result=UAbilityTask_WaitOverlap::WaitForOverlap(Z_Param_OwningAbility); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnHitCallback) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_HitComp); \
		P_GET_OBJECT(AActor,Z_Param_OtherActor); \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OtherComp); \
		P_GET_STRUCT(FVector,Z_Param_NormalImpulse); \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_Hit); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnHitCallback(Z_Param_HitComp,Z_Param_OtherActor,Z_Param_OtherComp,Z_Param_NormalImpulse,Z_Param_Out_Hit); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execWaitForOverlap) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_WaitOverlap**)Z_Param__Result=UAbilityTask_WaitOverlap::WaitForOverlap(Z_Param_OwningAbility); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnHitCallback) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_HitComp); \
		P_GET_OBJECT(AActor,Z_Param_OtherActor); \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OtherComp); \
		P_GET_STRUCT(FVector,Z_Param_NormalImpulse); \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_Hit); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnHitCallback(Z_Param_HitComp,Z_Param_OtherActor,Z_Param_OtherComp,Z_Param_NormalImpulse,Z_Param_Out_Hit); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitOverlap(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitOverlap_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitOverlap, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitOverlap)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_WaitOverlap(); \
	friend struct Z_Construct_UClass_UAbilityTask_WaitOverlap_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_WaitOverlap, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_WaitOverlap)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitOverlap(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitOverlap) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitOverlap); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitOverlap); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitOverlap(UAbilityTask_WaitOverlap&&); \
	NO_API UAbilityTask_WaitOverlap(const UAbilityTask_WaitOverlap&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_WaitOverlap(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_WaitOverlap(UAbilityTask_WaitOverlap&&); \
	NO_API UAbilityTask_WaitOverlap(const UAbilityTask_WaitOverlap&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_WaitOverlap); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_WaitOverlap); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_WaitOverlap)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_19_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h_22_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_WaitOverlap."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_WaitOverlap>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_WaitOverlap_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
