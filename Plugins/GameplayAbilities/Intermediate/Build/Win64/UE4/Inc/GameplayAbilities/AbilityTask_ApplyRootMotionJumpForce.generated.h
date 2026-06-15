// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayAbility;
struct FRotator;
enum class ERootMotionFinishVelocityMode : uint8;
struct FVector;
class UCurveVector;
class UCurveFloat;
class UAbilityTask_ApplyRootMotionJumpForce;
struct FHitResult;
#ifdef GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionJumpForce_generated_h
#error "AbilityTask_ApplyRootMotionJumpForce.generated.h already included, missing '#pragma once' in AbilityTask_ApplyRootMotionJumpForce.h"
#endif
#define GAMEPLAYABILITIES_AbilityTask_ApplyRootMotionJumpForce_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_15_DELEGATE \
static inline void FApplyRootMotionJumpForceDelegate_DelegateWrapper(const FMulticastScriptDelegate& ApplyRootMotionJumpForceDelegate) \
{ \
	ApplyRootMotionJumpForceDelegate.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execApplyRootMotionJumpForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FRotator,Z_Param_Rotation); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Distance); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Height); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_MinimumLandedTriggerTime); \
		P_GET_UBOOL(Z_Param_bFinishOnLanded); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TimeMappingCurve); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionJumpForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionJumpForce::ApplyRootMotionJumpForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_Rotation,Z_Param_Distance,Z_Param_Height,Z_Param_Duration,Z_Param_MinimumLandedTriggerTime,Z_Param_bFinishOnLanded,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish,Z_Param_PathOffsetCurve,Z_Param_TimeMappingCurve); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnLandedCallback) \
	{ \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_Hit); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnLandedCallback(Z_Param_Out_Hit); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execFinish) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->Finish(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execApplyRootMotionJumpForce) \
	{ \
		P_GET_OBJECT(UGameplayAbility,Z_Param_OwningAbility); \
		P_GET_PROPERTY(UNameProperty,Z_Param_TaskInstanceName); \
		P_GET_STRUCT(FRotator,Z_Param_Rotation); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Distance); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Height); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_MinimumLandedTriggerTime); \
		P_GET_UBOOL(Z_Param_bFinishOnLanded); \
		P_GET_ENUM(ERootMotionFinishVelocityMode,Z_Param_VelocityOnFinishMode); \
		P_GET_STRUCT(FVector,Z_Param_SetVelocityOnFinish); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClampVelocityOnFinish); \
		P_GET_OBJECT(UCurveVector,Z_Param_PathOffsetCurve); \
		P_GET_OBJECT(UCurveFloat,Z_Param_TimeMappingCurve); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilityTask_ApplyRootMotionJumpForce**)Z_Param__Result=UAbilityTask_ApplyRootMotionJumpForce::ApplyRootMotionJumpForce(Z_Param_OwningAbility,Z_Param_TaskInstanceName,Z_Param_Rotation,Z_Param_Distance,Z_Param_Height,Z_Param_Duration,Z_Param_MinimumLandedTriggerTime,Z_Param_bFinishOnLanded,ERootMotionFinishVelocityMode(Z_Param_VelocityOnFinishMode),Z_Param_SetVelocityOnFinish,Z_Param_ClampVelocityOnFinish,Z_Param_PathOffsetCurve,Z_Param_TimeMappingCurve); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnLandedCallback) \
	{ \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_Hit); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnLandedCallback(Z_Param_Out_Hit); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execFinish) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->Finish(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionJumpForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionJumpForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionJumpForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionJumpForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_INCLASS \
private: \
	static void StaticRegisterNativesUAbilityTask_ApplyRootMotionJumpForce(); \
	friend struct Z_Construct_UClass_UAbilityTask_ApplyRootMotionJumpForce_Statics; \
public: \
	DECLARE_CLASS(UAbilityTask_ApplyRootMotionJumpForce, UAbilityTask_ApplyRootMotion_Base, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilityTask_ApplyRootMotionJumpForce) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionJumpForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionJumpForce) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionJumpForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionJumpForce); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionJumpForce(UAbilityTask_ApplyRootMotionJumpForce&&); \
	NO_API UAbilityTask_ApplyRootMotionJumpForce(const UAbilityTask_ApplyRootMotionJumpForce&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilityTask_ApplyRootMotionJumpForce(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilityTask_ApplyRootMotionJumpForce(UAbilityTask_ApplyRootMotionJumpForce&&); \
	NO_API UAbilityTask_ApplyRootMotionJumpForce(const UAbilityTask_ApplyRootMotionJumpForce&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilityTask_ApplyRootMotionJumpForce); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilityTask_ApplyRootMotionJumpForce); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilityTask_ApplyRootMotionJumpForce)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__Rotation() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, Rotation); } \
	FORCEINLINE static uint32 __PPO__Distance() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, Distance); } \
	FORCEINLINE static uint32 __PPO__Height() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, Height); } \
	FORCEINLINE static uint32 __PPO__Duration() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, Duration); } \
	FORCEINLINE static uint32 __PPO__MinimumLandedTriggerTime() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, MinimumLandedTriggerTime); } \
	FORCEINLINE static uint32 __PPO__bFinishOnLanded() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, bFinishOnLanded); } \
	FORCEINLINE static uint32 __PPO__PathOffsetCurve() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, PathOffsetCurve); } \
	FORCEINLINE static uint32 __PPO__TimeMappingCurve() { return STRUCT_OFFSET(UAbilityTask_ApplyRootMotionJumpForce, TimeMappingCurve); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_22_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h_25_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilityTask_ApplyRootMotionJumpForce."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilityTask_ApplyRootMotionJumpForce>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_Tasks_AbilityTask_ApplyRootMotionJumpForce_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
