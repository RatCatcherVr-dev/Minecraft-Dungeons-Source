// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FVector;
class AActor;
class UGameplayAbility;
struct FGameplayAbilityTargetDataHandle;
enum class ERootMotionMoveToActorTargetOffsetType : uint8;
class UCurveFloat;
class UCurveVector;
enum class ERootMotionFinishVelocityMode : uint8;
class UAbilityTask_ApplyRootMotionMoveToActorForce;
#ifdef GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionMoveToActorForce_generated_h
#error "AbilityTask_ApplyRootMotionMoveToActorForce.generated.h already included, missing '#pragma once' in AbilityTask_ApplyRootMotionMoveToActorForce.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionMoveToActorForce_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_18_DELEGATE \
struct _Script_GameplayAbilities_eventApplyRootMotionMoveToActorForceDelegate_Parms \
{ \
	bool DestinationReached; \
	bool TimedOut; \
	FVector FinalTargetLocation; \
}; \
static inline void FApplyRootMotionMoveToActorForceDelegate_DelegateWrapper(const FMulticastScriptDelegate& ApplyRootMotionMoveToActorForceDelegate, bool DestinationReached, bool TimedOut, FVector FinalTargetLocation) \
{ \
	_Script_GameplayAbilities_eventApplyRootMotionMoveToActorForceDelegate_Parms Parms; \
	Parms.DestinationReached=DestinationReached ? true : false; \
	Parms.TimedOut=TimedOut ? true : false; \
	Parms.FinalTargetLocation=FinalTargetLocation; \
	ApplyRootMotionMoveToActorForceDelegate.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execOnRep_TargetLocation) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_TargetLocation(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnTargetActorSwapped) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_OriginalTarget); \
		P_GET_OBJECT(AActor,Z_Param_NewTarget); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnTargetActorSwapped(Z_Param_OriginalTarget,Z_Param_NewTarget); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execApplyRootMotionMoveToTargetDataActorForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetDataHandle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_TargetDataIndex); \
		P_GET_PROPERTY(UIntProperty,Z_Param_TargetActorIndex); \
		P_GET_STRUCT(FVector,Z_Param_TargetLocationOffset); \
		P_GET_ENUM(ERootMotionMoveToActorTargetOffsetType,Z_Param_OffsetAlignment); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedHorizontal); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedVertical); \
		P_GET_UBOOL(Z_Param_bSetNewMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_MovementMode); \
		P_GET_UBOOL(Z_Param_bRestrictSpeedToExpected); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TimeMappingCurve); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_GET_UBOOL(Z_Param_bDisableDestinationReachedInterrupt); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionMoveToActorForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToTargetDataActorForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_TargetDataHandle,Z_Param_TargetDataIndex,Z_Param_TargetActorIndex,Z_Param_TargetLocationOffset,ERootMotionMoveToActorTargetOffsetType(Z_Param_OffsetAlignment),Z_Param_Duration,Z_Param_TargetLerpSpeedHorizontal,Z_Param_TargetLerpSpeedVertical,Z_Param_bSetNewMovementMode,EMovementMode(Z_Param_MovementMode),Z_Param_bRestrictSpeedToExpected,Z_Param_PathOffsetCurve,Z_Param_TimeMappingCurve,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish,Z_Param_bDisableDestinationReachedInterrupt); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execApplyRootMotionMoveToActorForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_OBJECT(AActor,Z_Param_TargetActor); \
		P_GET_STRUCT(FVector,Z_Param_TargetLocationOffset); \
		P_GET_ENUM(ERootMotionMoveToActorTargetOffsetType,Z_Param_OffsetAlignment); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedHorizontal); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedVertical); \
		P_GET_UBOOL(Z_Param_bSetNewMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_MovementMode); \
		P_GET_UBOOL(Z_Param_bRestrictSpeedToExpected); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TimeMappingCurve); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_GET_UBOOL(Z_Param_bDisableDestinationReachedInterrupt); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionMoveToActorForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToActorForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_TargetActor,Z_Param_TargetLocationOffset,ERootMotionMoveToActorTargetOffsetType(Z_Param_OffsetAlignment),Z_Param_Duration,Z_Param_TargetLerpSpeedHorizontal,Z_Param_TargetLerpSpeedVertical,Z_Param_bSetNewMovementMode,EMovementMode(Z_Param_MovementMode),Z_Param_bRestrictSpeedToExpected,Z_Param_PathOffsetCurve,Z_Param_TimeMappingCurve,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish,Z_Param_bDisableDestinationReachedInterrupt); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execOnRep_TargetLocation) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_TargetLocation(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnTargetActorSwapped) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_OriginalTarget); \
		P_GET_OBJECT(AActor,Z_Param_NewTarget); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnTargetActorSwapped(Z_Param_OriginalTarget,Z_Param_NewTarget); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execApplyRootMotionMoveToTargetDataActorForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetDataHandle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_TargetDataIndex); \
		P_GET_PROPERTY(UIntProperty,Z_Param_TargetActorIndex); \
		P_GET_STRUCT(FVector,Z_Param_TargetLocationOffset); \
		P_GET_ENUM(ERootMotionMoveToActorTargetOffsetType,Z_Param_OffsetAlignment); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedHorizontal); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedVertical); \
		P_GET_UBOOL(Z_Param_bSetNewMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_MovementMode); \
		P_GET_UBOOL(Z_Param_bRestrictSpeedToExpected); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TimeMappingCurve); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_GET_UBOOL(Z_Param_bDisableDestinationReachedInterrupt); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionMoveToActorForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToTargetDataActorForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_TargetDataHandle,Z_Param_TargetDataIndex,Z_Param_TargetActorIndex,Z_Param_TargetLocationOffset,ERootMotionMoveToActorTargetOffsetType(Z_Param_OffsetAlignment),Z_Param_Duration,Z_Param_TargetLerpSpeedHorizontal,Z_Param_TargetLerpSpeedVertical,Z_Param_bSetNewMovementMode,EMovementMode(Z_Param_MovementMode),Z_Param_bRestrictSpeedToExpected,Z_Param_PathOffsetCurve,Z_Param_TimeMappingCurve,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish,Z_Param_bDisableDestinationReachedInterrupt); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execApplyRootMotionMoveToActorForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_OBJECT(AActor,Z_Param_TargetActor); \
		P_GET_STRUCT(FVector,Z_Param_TargetLocationOffset); \
		P_GET_ENUM(ERootMotionMoveToActorTargetOffsetType,Z_Param_OffsetAlignment); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedHorizontal); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TargetLerpSpeedVertical); \
		P_GET_UBOOL(Z_Param_bSetNewMovementMode); \
		P_GET_PROPERTY(UByteProperty,Z_Param_MovementMode); \
		P_GET_UBOOL(Z_Param_bRestrictSpeedToExpected); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TimeMappingCurve); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_GET_UBOOL(Z_Param_bDisableDestinationReachedInterrupt); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionMoveToActorForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToActorForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_TargetActor,Z_Param_TargetLocationOffset,ERootMotionMoveToActorTargetOffsetType(Z_Param_OffsetAlignment),Z_Param_Duration,Z_Param_TargetLerpSpeedHorizontal,Z_Param_TargetLerpSpeedVertical,Z_Param_bSetNewMovementMode,EMovementMode(Z_Param_MovementMode),Z_Param_bRestrictSpeedToExpected,Z_Param_PathOffsetCurve,Z_Param_TimeMappingCurve,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish,Z_Param_bDisableDestinationReachedInterrupt); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionMoveToActorForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionMoveToActorForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionMoveToActorForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionMoveToActorForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionMoveToActorForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionMoveToActorForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionMoveToActorForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionMoveToActorForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToActorForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionMoveToActorForce) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionMoveToActorForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionMoveToActorForce); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToActorForce(UAbilityTask_ApplyRootMotionMoveToActorForce&&); \
	NO_API UAbilityTask_ApplyRootMotionMoveToActorForce(const UAbilityTask_ApplyRootMotionMoveToActorForce&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToActorForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionMoveToActorForce(UAbilityTask_ApplyRootMotionMoveToActorForce&&); \
	NO_API UAbilityTask_ApplyRootMotionMoveToActorForce(const UAbilityTask_ApplyRootMotionMoveToActorForce&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionMoveToActorForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionMoveToActorForce); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionMoveToActorForce)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__StartLocation() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, StartLocation); } \
	FORCEINLINE static uint32 __PPO__TargetLocation() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, TargetLocation); } \
	FORCEINLINE static uint32 __PPO__TargetActor() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, TargetActor); } \
	FORCEINLINE static uint32 __PPO__TargetLocationOffset() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, TargetLocationOffset); } \
	FORCEINLINE static uint32 __PPO__OffsetAlignment() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, OffsetAlignment); } \
	FORCEINLINE static uint32 __PPO__Duration() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, Duration); } \
	FORCEINLINE static uint32 __PPO__bDisableDestinationReachedInterrupt() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, bDisableDestinationReachedInterrupt); } \
	FORCEINLINE static uint32 __PPO__bSetNewMovementMode() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, bSetNewMovementMode); } \
	FORCEINLINE static uint32 __PPO__NewMovementMode() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, NewMovementMode); } \
	FORCEINLINE static uint32 __PPO__bRestrictSpeedToExpected() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, bRestrictSpeedToExpected); } \
	FORCEINLINE static uint32 __PPO__PathOffsetCurve() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, PathOffsetCurve); } \
	FORCEINLINE static uint32 __PPO__TimeMappingCurve() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, TimeMappingCurve); } \
	FORCEINLINE static uint32 __PPO__TargetLerpSpeedHorizontalCurve() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, TargetLerpSpeedHorizontalCurve); } \
	FORCEINLINE static uint32 __PPO__TargetLerpSpeedVerticalCurve() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionMoveToActorForce, TargetLerpSpeedVerticalCurve); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_36_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h_39_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_ApplyRootMotionMoveToActorForce."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_ApplyRootMotionMoveToActorForce>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionMoveToActorForce_h


#define FOREACH_ENUM_EROOTMOTIONMOVETOACTORTARGETOFFSETTYPE(op) \
	op(ERootMotionMoveToActorTargetOffsetType::AlignFromTargetToSource) \
	op(ERootMotionMoveToActorTargetOffsetType::AlignToTargetForward) \
	op(ERootMotionMoveToActorTargetOffsetType::AlignToWorldSpace) 

enum class ERootMotionMoveToActorTargetOffsetType : uint8;
template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<ERootMotionMoveToActorTargetOffsetType>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
