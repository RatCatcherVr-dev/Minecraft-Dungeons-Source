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
class AActor;
class UCurveFloat;
struct FRotator;
enum class ERootMotionFinishVelocityMode : uint8;
class UAbilityTask_ApplyRootMotionRadialForce;
#ifdef GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionRadialForce_generated_h
#error "AbilityTask_ApplyRootMotionRadialForce.generated.h already included, missing '#pragma once' in AbilityTask_ApplyRootMotionRadialForce.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionRadialForce_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_15_DELEGATE \
static inline void FApplyRootMotionRadialForceDelegate_DelegateWrapper(const FMulticastScriptDelegate& ApplyRootMotionRadialForceDelegate) \
{ \
	ApplyRootMotionRadialForceDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execApplyRootMotionRadialForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FVector,Z_Param_Location); \
		P_GET_OBJECT(AActor,Z_Param_LocationActor); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Strength); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Radius); \
		P_GET_UBOOL(Z_Param_bIsPush); \
		P_GET_UBOOL(Z_Param_bIsAdditive); \
		P_GET_UBOOL(Z_Param_bNoZForce); \
		P_GET_OBJECT(UCurveFloat,Z_Param_StrengthDistanceFalloff); \
		P_GET_OBJECT(UCurveFloat,Z_Param_StrengthOverTime); \
		P_GET_UBOOL(Z_Param_bUseFixedWorldDirection); \
		P_GET_STRUCT(FRotator,Z_Param_FixedWorldDirection); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionRadialForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionRadialForce::ApplyRootMotionRadialForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_Location,Z_Param_LocationActor,Z_Param_Strength,Z_Param_Duration,Z_Param_Radius,Z_Param_bIsPush,Z_Param_bIsAdditive,Z_Param_bNoZForce,Z_Param_StrengthDistanceFalloff,Z_Param_StrengthOverTime,Z_Param_bUseFixedWorldDirection,Z_Param_FixedWorldDirection,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execApplyRootMotionRadialForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FVector,Z_Param_Location); \
		P_GET_OBJECT(AActor,Z_Param_LocationActor); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Strength); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Radius); \
		P_GET_UBOOL(Z_Param_bIsPush); \
		P_GET_UBOOL(Z_Param_bIsAdditive); \
		P_GET_UBOOL(Z_Param_bNoZForce); \
		P_GET_OBJECT(UCurveFloat,Z_Param_StrengthDistanceFalloff); \
		P_GET_OBJECT(UCurveFloat,Z_Param_StrengthOverTime); \
		P_GET_UBOOL(Z_Param_bUseFixedWorldDirection); \
		P_GET_STRUCT(FRotator,Z_Param_FixedWorldDirection); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionRadialForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionRadialForce::ApplyRootMotionRadialForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_Location,Z_Param_LocationActor,Z_Param_Strength,Z_Param_Duration,Z_Param_Radius,Z_Param_bIsPush,Z_Param_bIsAdditive,Z_Param_bNoZForce,Z_Param_StrengthDistanceFalloff,Z_Param_StrengthOverTime,Z_Param_bUseFixedWorldDirection,Z_Param_FixedWorldDirection,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionRadialForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionRadialForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionRadialForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionRadialForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionRadialForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionRadialForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionRadialForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionRadialForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionRadialForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionRadialForce) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionRadialForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionRadialForce); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionRadialForce(UAbilityTask_ApplyRootMotionRadialForce&&); \
	NO_API UAbilityTask_ApplyRootMotionRadialForce(const UAbilityTask_ApplyRootMotionRadialForce&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionRadialForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionRadialForce(UAbilityTask_ApplyRootMotionRadialForce&&); \
	NO_API UAbilityTask_ApplyRootMotionRadialForce(const UAbilityTask_ApplyRootMotionRadialForce&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionRadialForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionRadialForce); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionRadialForce)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__Location() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, Location); } \
	FORCEINLINE static uint32 __PPO__LocationActor() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, LocationActor); } \
	FORCEINLINE static uint32 __PPO__Strength() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, Strength); } \
	FORCEINLINE static uint32 __PPO__Duration() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, Duration); } \
	FORCEINLINE static uint32 __PPO__Radius() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, Radius); } \
	FORCEINLINE static uint32 __PPO__bIsPush() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, bIsPush); } \
	FORCEINLINE static uint32 __PPO__bIsAdditive() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, bIsAdditive); } \
	FORCEINLINE static uint32 __PPO__bNoZForce() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, bNoZForce); } \
	FORCEINLINE static uint32 __PPO__StrengthDistanceFalloff() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, StrengthDistanceFalloff); } \
	FORCEINLINE static uint32 __PPO__StrengthOverTime() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, StrengthOverTime); } \
	FORCEINLINE static uint32 __PPO__bUseFixedWorldDirection() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, bUseFixedWorldDirection); } \
	FORCEINLINE static uint32 __PPO__FixedWorldDirection() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionRadialForce, FixedWorldDirection); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_22_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h_25_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_ApplyRootMotionRadialForce."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_ApplyRootMotionRadialForce>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionRadialForce_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
