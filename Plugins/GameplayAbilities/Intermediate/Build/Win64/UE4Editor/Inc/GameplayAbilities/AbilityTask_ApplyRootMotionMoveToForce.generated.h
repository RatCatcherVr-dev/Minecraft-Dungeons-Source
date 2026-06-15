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
class UCurveVector;
enum class ERootMotionFinishVelocityMode : uint8;
class UAbilityTask_ApplyRootMotionMoveToForce;
#ifdef GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionMoveToForce_generated_h
#error "AbilityTask_ApplyRootMotionMoveToForce.generated.h already included, missing '#pragma once' in AbilityTask_ApplyRootMotionMoveToForce.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionMoveToForce_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_15_DELEGATE \
static inline void FApplyRootMotionMoveToForceDelegate_DelegateWrapper(const FMulticastScriptDelegate& ApplyRootMotionMoveToForceDelegate) \
{ \
	ApplyRootMotionMoveToForceDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execApplyRootMotionMoveToForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FVector,Z_Param_TargetLocation); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_UBOOL(Z_Param_bSetNewMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_MovementMode); \
		P_GET_UBOOL(Z_Param_bRestrictSpeedToExpected); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionMoveToForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_TargetLocation,Z_Param_Duration,Z_Param_bSetNewMovementMode,EMovementMode(Z_Param_MovementMode),Z_Param_bRestrictSpeedToExpected,Z_Param_PathOffsetCurve,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execApplyRootMotionMoveToForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FVector,Z_Param_TargetLocation); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_UBOOL(Z_Param_bSetNewMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_MovementMode); \
		P_GET_UBOOL(Z_Param_bRestrictSpeedToExpected); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionMoveToForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_TargetLocation,Z_Param_Duration,Z_Param_bSetNewMovementMode,EMovementMode(Z_Param_MovementMode),Z_Param_bRestrictSpeedToExpected,Z_Param_PathOffsetCurve,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionMoveToForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionMoveToForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionMoveToForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionMoveToForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionMoveToForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionMoveToForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionMoveToForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionMoveToForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionMoveToForce) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionMoveToForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionMoveToForce); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToForce(UAbilityTask_ApplyRootMotionMoveToForce&&); \
	NO_API UAbilityTask_ApplyRootMotionMoveToForce(const UAbilityTask_ApplyRootMotionMoveToForce&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToForce(UAbilityTask_ApplyRootMotionMoveToForce&&); \
	NO_API UAbilityTask_ApplyRootMotionMoveToForce(const UAbilityTask_ApplyRootMotionMoveToForce&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionMoveToForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionMoveToForce); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionMoveToForce)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__StartLocation() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToForce, StartLocation); } \
	FORCEINLINE static uint32 __PPO__TargetLocation() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToForce, TargetLocation); } \
	FORCEINLINE static uint32 __PPO__Duration() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToForce, Duration); } \
	FORCEINLINE static uint32 __PPO__bSetNewMovementMode() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToForce, bSetNewMovementMode); } \
	FORCEINLINE static uint32 __PPO__NewMovementMode() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToForce, NewMovementMode); } \
	FORCEINLINE static uint32 __PPO__bRestrictSpeedToExpected() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToForce, bRestrictSpeedToExpected); } \
	FORCEINLINE static uint32 __PPO__PathOffsetCurve() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToForce, PathOffsetCurve); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_22_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h_25_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_ApplyRootMotionMoveToForce."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_ApplyRootMotionMoveToForce>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToForce_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
