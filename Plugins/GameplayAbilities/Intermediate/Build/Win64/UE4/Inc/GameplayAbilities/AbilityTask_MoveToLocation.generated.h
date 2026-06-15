// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
struct FVector;
class UCurveFloat;
class UCurveVector;
class UAbilityTask_MoveToLocation;
#ifdef GAMEPLAYABILITIES_AbilityTask_MoveToLocation_generated_h
#error "AbilityTask_MoveToLocation.generated.h already included, missing '#pragma once' in AbilityTask_MoveToLocation.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_MoveToLocation_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_13_DELEGATE \
static inline void FMoveToLocationDelegate_DelegateWrapper(const FMulticastScriptDelegate& MoveToLocationDelegate) \
{ \
	MoveToLocationDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execMoveToLocation) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FVector,Z_Param_Location); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_OBJECT(UCurveFloat,Z_Param_OptionalInterpolationCurve); \
		P_GET_OBJECT(UCurveVector,Z_Param_OptionalVectorInterpolationCurve); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_MoveToLocation**)Z_Param__Result=UAbilityTask_MoveToLocation::MoveToLocation(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_Location,Z_Param_Duration,Z_Param_OptionalInterpolationCurve,Z_Param_OptionalVectorInterpolationCurve); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execMoveToLocation) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FVector,Z_Param_Location); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_OBJECT(UCurveFloat,Z_Param_OptionalInterpolationCurve); \
		P_GET_OBJECT(UCurveVector,Z_Param_OptionalVectorInterpolationCurve); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_MoveToLocation**)Z_Param__Result=UAbilityTask_MoveToLocation::MoveToLocation(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_Location,Z_Param_Duration,Z_Param_OptionalInterpolationCurve,Z_Param_OptionalVectorInterpolationCurve); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_MoveToLocation(); \
	friend struct Z_Construct_UClass_UAbilityTask_MoveToLocation_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_MoveToLocation, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_MoveToLocation) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_MoveToLocation(); \
	friend struct Z_Construct_UClass_UAbilityTask_MoveToLocation_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_MoveToLocation, UAbilityTask, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_MoveToLocation) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_MoveToLocation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_MoveToLocation) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_MoveToLocation); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_MoveToLocation); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_MoveToLocation(UAbilityTask_MoveToLocation&&); \
	NO_API UAbilityTask_MoveToLocation(const UAbilityTask_MoveToLocation&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_MoveToLocation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_MoveToLocation(UAbilityTask_MoveToLocation&&); \
	NO_API UAbilityTask_MoveToLocation(const UAbilityTask_MoveToLocation&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_MoveToLocation); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_MoveToLocation); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_MoveToLocation)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__StartLocation() { return STRUCT_OFFSET(UAbilityTask_MoveToLocation, StartLocation); } \
	FORCEINLINE static uint32 __PPO__TargetLocation() { return STRUCT_OFFSET(UAbilityTask_MoveToLocation, TargetLocation); } \
	FORCEINLINE static uint32 __PPO__DurationOfMovement() { return STRUCT_OFFSET(UAbilityTask_MoveToLocation, DurationOfMovement); } \
	FORCEINLINE static uint32 __PPO__LerpCurve() { return STRUCT_OFFSET(UAbilityTask_MoveToLocation, LerpCurve); } \
	FORCEINLINE static uint32 __PPO__LerpCurveVector() { return STRUCT_OFFSET(UAbilityTask_MoveToLocation, LerpCurveVector); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_30_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h_33_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_MoveToLocation."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_MoveToLocation>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_MoveToLocation_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
