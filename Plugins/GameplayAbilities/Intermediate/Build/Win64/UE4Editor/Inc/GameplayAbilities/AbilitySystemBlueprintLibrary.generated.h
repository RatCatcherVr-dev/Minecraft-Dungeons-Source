// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FActiveGameplayEffectHandle;
class UObject;
struct FGameplayEffectSpecHandle;
struct FGameplayAttribute;
struct FGameplayEffectContextHandle;
class UGameplayEffect;
struct FGameplayTagContainer;
struct FGameplayTag;
struct FGameplayCueParameters;
struct FVector;
class AActor;
class UPhysicalMaterial;
class USceneComponent;
struct FGameplayTagRequirements;
struct FTransform;
class IGameplayCueInterface;
struct FHitResult;
struct FGameplayAbilityTargetDataHandle;
struct FGameplayTargetDataFilter;
struct FGameplayTargetDataFilterHandle;
struct FGameplayAbilityTargetingLocationInfo;
class UAbilitySystemComponent;
struct FGameplayEventData;
#ifdef GAMEPLAYABILITIES_AbilitySystemBlueprintLibrary_generated_h
#error "AbilitySystemBlueprintLibrary.generated.h already included, missing '#pragma once' in AbilitySystemBlueprintLibrary.h"
#endif
#define GAMEPLAYABILITIES_AbilitySystemBlueprintLibrary_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectDebugString) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FString*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectDebugString(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectRemainingDuration) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectRemainingDuration(Z_Param_WorldContextObject,Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectTotalDuration) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectTotalDuration(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectExpectedEndTime) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectExpectedEndTime(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectStartTime) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStartTime(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectStackLimitCount) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackLimitCount(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectStackCount) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackCount(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetModifiedAttributeMagnitude) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetModifiedAttributeMagnitude(Z_Param_SpecHandle,Z_Param_Attribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAllLinkedGameplayEffectSpecHandles) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FGameplayEffectSpecHandle>*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetAllLinkedGameplayEffectSpecHandles(Z_Param_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetEffectContext) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetEffectContext(Z_Param_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetStackCountToMax) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::SetStackCountToMax(Z_Param_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetStackCount) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StackCount); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::SetStackCount(Z_Param_SpecHandle,Z_Param_StackCount); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddLinkedGameplayEffect) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_OBJECT(UClass,Z_Param_LinkedGameplayEffect); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddLinkedGameplayEffect(Z_Param_SpecHandle,Z_Param_LinkedGameplayEffect); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddLinkedGameplayEffectSpec) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_LinkedGameplayEffectSpec); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddLinkedGameplayEffectSpec(Z_Param_SpecHandle,Z_Param_LinkedGameplayEffectSpec); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddAssetTags) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_NewGameplayTags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddAssetTags(Z_Param_SpecHandle,Z_Param_NewGameplayTags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddAssetTag) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_NewGameplayTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddAssetTag(Z_Param_SpecHandle,Z_Param_NewGameplayTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddGrantedTags) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_NewGameplayTags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddGrantedTags(Z_Param_SpecHandle,Z_Param_NewGameplayTags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddGrantedTag) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_NewGameplayTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddGrantedTag(Z_Param_SpecHandle,Z_Param_NewGameplayTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetDuration) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::SetDuration(Z_Param_SpecHandle,Z_Param_Duration); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAssignTagSetByCallerMagnitude) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_DataTag); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Magnitude); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Z_Param_SpecHandle,Z_Param_DataTag,Z_Param_Magnitude); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAssignSetByCallerMagnitude) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_PROPERTY(UNameProperty,Z_Param_DataName); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Magnitude); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude(Z_Param_SpecHandle,Z_Param_DataName,Z_Param_Magnitude); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBreakGameplayCueParameters) \
	{ \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_GET_PROPERTY_REF(UFloatProperty,Z_Param_Out_NormalizedMagnitude); \
		P_GET_PROPERTY_REF(UFloatProperty,Z_Param_Out_RawMagnitude); \
		P_GET_STRUCT_REF(FGameplayEffectContextHandle,Z_Param_Out_EffectContext); \
		P_GET_STRUCT_REF(FGameplayTag,Z_Param_Out_MatchedTagName); \
		P_GET_STRUCT_REF(FGameplayTag,Z_Param_Out_OriginalTag); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_AggregatedSourceTags); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_AggregatedTargetTags); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Location); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Normal); \
		P_GET_OBJECT_REF(AActor,Z_Param_Out_Instigator); \
		P_GET_OBJECT_REF(AActor,Z_Param_Out_EffectCauser); \
		P_GET_OBJECT_REF(UObject,Z_Param_Out_SourceObject); \
		P_GET_OBJECT_REF(UPhysicalMaterial,Z_Param_Out_PhysicalMaterial); \
		P_GET_PROPERTY_REF(UIntProperty,Z_Param_Out_GameplayEffectLevel); \
		P_GET_PROPERTY_REF(UIntProperty,Z_Param_Out_AbilityLevel); \
		P_GET_OBJECT_REF(USceneComponent,Z_Param_Out_TargetAttachComponent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::BreakGameplayCueParameters(Z_Param_Out_Parameters,Z_Param_Out_NormalizedMagnitude,Z_Param_Out_RawMagnitude,Z_Param_Out_EffectContext,Z_Param_Out_MatchedTagName,Z_Param_Out_OriginalTag,Z_Param_Out_AggregatedSourceTags,Z_Param_Out_AggregatedTargetTags,Z_Param_Out_Location,Z_Param_Out_Normal,Z_Param_Out_Instigator,Z_Param_Out_EffectCauser,Z_Param_Out_SourceObject,Z_Param_Out_PhysicalMaterial,Z_Param_Out_GameplayEffectLevel,Z_Param_Out_AbilityLevel,Z_Param_Out_TargetAttachComponent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeGameplayCueParameters) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_NormalizedMagnitude); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_RawMagnitude); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_GET_STRUCT(FGameplayTag,Z_Param_MatchedTagName); \
		P_GET_STRUCT(FGameplayTag,Z_Param_OriginalTag); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_AggregatedSourceTags); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_AggregatedTargetTags); \
		P_GET_STRUCT(FVector,Z_Param_Location); \
		P_GET_STRUCT(FVector,Z_Param_Normal); \
		P_GET_OBJECT(AActor,Z_Param_Instigator); \
		P_GET_OBJECT(AActor,Z_Param_EffectCauser); \
		P_GET_OBJECT(UObject,Z_Param_SourceObject); \
		P_GET_OBJECT(UPhysicalMaterial,Z_Param_PhysicalMaterial); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameplayEffectLevel); \
		P_GET_PROPERTY(UIntProperty,Z_Param_AbilityLevel); \
		P_GET_OBJECT(USceneComponent,Z_Param_TargetAttachComponent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayCueParameters*)Z_Param__Result=UAbilitySystemBlueprintLibrary::MakeGameplayCueParameters(Z_Param_NormalizedMagnitude,Z_Param_RawMagnitude,Z_Param_EffectContext,Z_Param_MatchedTagName,Z_Param_OriginalTag,Z_Param_AggregatedSourceTags,Z_Param_AggregatedTargetTags,Z_Param_Location,Z_Param_Normal,Z_Param_Instigator,Z_Param_EffectCauser,Z_Param_SourceObject,Z_Param_PhysicalMaterial,Z_Param_GameplayEffectLevel,Z_Param_AbilityLevel,Z_Param_TargetAttachComponent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execDoesGameplayCueMeetTagRequirements) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_STRUCT_REF(FGameplayTagRequirements,Z_Param_Out_SourceTagReqs); \
		P_GET_STRUCT_REF(FGameplayTagRequirements,Z_Param_Out_TargetTagReqs); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::DoesGameplayCueMeetTagRequirements(Z_Param_Parameters,Z_Param_Out_SourceTagReqs,Z_Param_Out_TargetTagReqs); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayCueDirection) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_TargetActor); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Direction); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetGameplayCueDirection(Z_Param_TargetActor,Z_Param_Parameters,Z_Param_Out_Direction); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayCueEndLocationAndNormal) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_TargetActor); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Location); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Normal); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetGameplayCueEndLocationAndNormal(Z_Param_TargetActor,Z_Param_Parameters,Z_Param_Out_Location,Z_Param_Out_Normal); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetOrigin) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FVector*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetOrigin(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetInstigatorTransform) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTransform*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetInstigatorTransform(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetInstigatorActor) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetInstigatorActor(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execForwardGameplayCueToTarget) \
	{ \
		P_GET_TINTERFACE(IGameplayCueInterface,Z_Param_TargetCueInterface); \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::ForwardGameplayCueToTarget(Z_Param_TargetCueInterface,EGameplayCueEvent::Type(Z_Param_EventType),Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execHasHitResult) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::HasHitResult(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetHitResult) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FHitResult*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetHitResult(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorByIndex) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActorByIndex(Z_Param_Parameters,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorCount) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActorCount(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsInstigatorLocallyControlledPlayer) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::IsInstigatorLocallyControlledPlayer(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsInstigatorLocallyControlled) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::IsInstigatorLocallyControlled(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetSourceObject) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UObject**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetSourceObject(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetEffectCauser) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetEffectCauser(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetOriginalInstigatorActor) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetOriginalInstigatorActor(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetInstigatorActor) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetInstigatorActor(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextSetOrigin) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_GET_STRUCT(FVector,Z_Param_Origin); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::EffectContextSetOrigin(Z_Param_EffectContext,Z_Param_Origin); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetOrigin) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FVector*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetOrigin(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextAddHitResult) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_GET_STRUCT(FHitResult,Z_Param_HitResult); \
		P_GET_UBOOL(Z_Param_bReset); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::EffectContextAddHitResult(Z_Param_EffectContext,Z_Param_HitResult,Z_Param_bReset); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextHasHitResult) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextHasHitResult(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetHitResult) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FHitResult*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetHitResult(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextIsInstigatorLocallyControlled) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextIsInstigatorLocallyControlled(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextIsValid) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextIsValid(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTargetDataEndPointTransform) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTransform*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetTargetDataEndPointTransform(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTargetDataEndPoint) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FVector*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetTargetDataEndPoint(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasEndPoint) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasEndPoint(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTargetDataOrigin) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTransform*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetTargetDataOrigin(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasOrigin) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasOrigin(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetHitResultFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_HitResult); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FHitResult*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Z_Param_Out_HitResult,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasHitResult) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_HitResult); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(Z_Param_Out_HitResult,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasActor) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasActor(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execDoesTargetDataContainActor) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::DoesTargetDataContainActor(Z_Param_Out_TargetData,Z_Param_Index,Z_Param_Actor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAllActorsFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<AActor*>*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(Z_Param_Out_TargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorsFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<AActor*>*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execCloneSpecHandle) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_InNewInstigator); \
		P_GET_OBJECT(AActor,Z_Param_InEffectCauser); \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_GameplayEffectSpecHandle_Clone); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::CloneSpecHandle(Z_Param_InNewInstigator,Z_Param_InEffectCauser,Z_Param_GameplayEffectSpecHandle_Clone); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeSpecHandle) \
	{ \
		P_GET_OBJECT(UGameplayEffect,Z_Param_InGameplayEffect); \
		P_GET_OBJECT(AActor,Z_Param_InInstigator); \
		P_GET_OBJECT(AActor,Z_Param_InEffectCauser); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_InLevel); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::MakeSpecHandle(Z_Param_InGameplayEffect,Z_Param_InInstigator,Z_Param_InEffectCauser,Z_Param_InLevel); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeFilterHandle) \
	{ \
		P_GET_STRUCT(FGameplayTargetDataFilter,Z_Param_Filter); \
		P_GET_OBJECT(AActor,Z_Param_FilterActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayTargetDataFilterHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::MakeFilterHandle(Z_Param_Filter,Z_Param_FilterActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execFilterTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetDataHandle); \
		P_GET_STRUCT(FGameplayTargetDataFilterHandle,Z_Param_ActorFilterClass); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::FilterTargetData(Z_Param_Out_TargetDataHandle,Z_Param_ActorFilterClass); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromActorArray) \
	{ \
		P_GET_TARRAY_REF(AActor*,Z_Param_Out_ActorArray); \
		P_GET_UBOOL(Z_Param_OneTargetPerHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(Z_Param_Out_ActorArray,Z_Param_OneTargetPerHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromActor) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Z_Param_Actor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetDataCountFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(Z_Param_Out_TargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromHitResult) \
	{ \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_HitResult); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Z_Param_Out_HitResult); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromLocations) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetingLocationInfo,Z_Param_Out_SourceLocation); \
		P_GET_STRUCT_REF(FGameplayAbilityTargetingLocationInfo,Z_Param_Out_TargetLocation); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(Z_Param_Out_SourceLocation,Z_Param_Out_TargetLocation); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAppendTargetDataHandle) \
	{ \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetHandle); \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_HandleToAdd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AppendTargetDataHandle(Z_Param_TargetHandle,Z_Param_Out_HandleToAdd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNotEqual_GameplayAttributeGameplayAttribute) \
	{ \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeA); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeB); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::NotEqual_GameplayAttributeGameplayAttribute(Z_Param_AttributeA,Z_Param_AttributeB); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEqualEqual_GameplayAttributeGameplayAttribute) \
	{ \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeA); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeB); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EqualEqual_GameplayAttributeGameplayAttribute(Z_Param_AttributeA,Z_Param_AttributeB); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEvaluateAttributeValueWithTagsAndBase) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystem); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_SourceTags); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_TargetTags); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_BaseValue); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccess); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTagsAndBase(Z_Param_AbilitySystem,Z_Param_Attribute,Z_Param_Out_SourceTags,Z_Param_Out_TargetTags,Z_Param_BaseValue,Z_Param_Out_bSuccess); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEvaluateAttributeValueWithTags) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystem); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_SourceTags); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_TargetTags); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccess); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTags(Z_Param_AbilitySystem,Z_Param_Attribute,Z_Param_Out_SourceTags,Z_Param_Out_TargetTags,Z_Param_Out_bSuccess); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttributeBaseFromAbilitySystemComponent) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystemComponent); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttributeBaseFromAbilitySystemComponent(Z_Param_AbilitySystemComponent,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttributeBase) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttributeBase(Z_Param_Actor,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttributeFromAbilitySystemComponent) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystem); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(Z_Param_AbilitySystem,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttribute) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttribute(Z_Param_Actor,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsValid) \
	{ \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::IsValid(Z_Param_Attribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSendGameplayEventToActor) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_GET_STRUCT(FGameplayTag,Z_Param_EventTag); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_Payload); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Z_Param_Actor,Z_Param_EventTag,Z_Param_Payload); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAbilitySystemComponent) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilitySystemComponent**)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Z_Param_Actor); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectDebugString) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FString*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectDebugString(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectRemainingDuration) \
	{ \
		P_GET_OBJECT(UObject,Z_Param_WorldContextObject); \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectRemainingDuration(Z_Param_WorldContextObject,Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectTotalDuration) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectTotalDuration(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectExpectedEndTime) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectExpectedEndTime(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectStartTime) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStartTime(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectStackLimitCount) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackLimitCount(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveGameplayEffectStackCount) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackCount(Z_Param_ActiveHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetModifiedAttributeMagnitude) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetModifiedAttributeMagnitude(Z_Param_SpecHandle,Z_Param_Attribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAllLinkedGameplayEffectSpecHandles) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FGameplayEffectSpecHandle>*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetAllLinkedGameplayEffectSpecHandles(Z_Param_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetEffectContext) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetEffectContext(Z_Param_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetStackCountToMax) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::SetStackCountToMax(Z_Param_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetStackCount) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StackCount); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::SetStackCount(Z_Param_SpecHandle,Z_Param_StackCount); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddLinkedGameplayEffect) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_OBJECT(UClass,Z_Param_LinkedGameplayEffect); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddLinkedGameplayEffect(Z_Param_SpecHandle,Z_Param_LinkedGameplayEffect); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddLinkedGameplayEffectSpec) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_LinkedGameplayEffectSpec); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddLinkedGameplayEffectSpec(Z_Param_SpecHandle,Z_Param_LinkedGameplayEffectSpec); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddAssetTags) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_NewGameplayTags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddAssetTags(Z_Param_SpecHandle,Z_Param_NewGameplayTags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddAssetTag) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_NewGameplayTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddAssetTag(Z_Param_SpecHandle,Z_Param_NewGameplayTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddGrantedTags) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_NewGameplayTags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddGrantedTags(Z_Param_SpecHandle,Z_Param_NewGameplayTags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAddGrantedTag) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_NewGameplayTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AddGrantedTag(Z_Param_SpecHandle,Z_Param_NewGameplayTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetDuration) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Duration); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::SetDuration(Z_Param_SpecHandle,Z_Param_Duration); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAssignTagSetByCallerMagnitude) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_DataTag); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Magnitude); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(Z_Param_SpecHandle,Z_Param_DataTag,Z_Param_Magnitude); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAssignSetByCallerMagnitude) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_SpecHandle); \
		P_GET_PROPERTY(UNameProperty,Z_Param_DataName); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Magnitude); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude(Z_Param_SpecHandle,Z_Param_DataName,Z_Param_Magnitude); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBreakGameplayCueParameters) \
	{ \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_GET_PROPERTY_REF(UFloatProperty,Z_Param_Out_NormalizedMagnitude); \
		P_GET_PROPERTY_REF(UFloatProperty,Z_Param_Out_RawMagnitude); \
		P_GET_STRUCT_REF(FGameplayEffectContextHandle,Z_Param_Out_EffectContext); \
		P_GET_STRUCT_REF(FGameplayTag,Z_Param_Out_MatchedTagName); \
		P_GET_STRUCT_REF(FGameplayTag,Z_Param_Out_OriginalTag); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_AggregatedSourceTags); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_AggregatedTargetTags); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Location); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Normal); \
		P_GET_OBJECT_REF(AActor,Z_Param_Out_Instigator); \
		P_GET_OBJECT_REF(AActor,Z_Param_Out_EffectCauser); \
		P_GET_OBJECT_REF(UObject,Z_Param_Out_SourceObject); \
		P_GET_OBJECT_REF(UPhysicalMaterial,Z_Param_Out_PhysicalMaterial); \
		P_GET_PROPERTY_REF(UIntProperty,Z_Param_Out_GameplayEffectLevel); \
		P_GET_PROPERTY_REF(UIntProperty,Z_Param_Out_AbilityLevel); \
		P_GET_OBJECT_REF(USceneComponent,Z_Param_Out_TargetAttachComponent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::BreakGameplayCueParameters(Z_Param_Out_Parameters,Z_Param_Out_NormalizedMagnitude,Z_Param_Out_RawMagnitude,Z_Param_Out_EffectContext,Z_Param_Out_MatchedTagName,Z_Param_Out_OriginalTag,Z_Param_Out_AggregatedSourceTags,Z_Param_Out_AggregatedTargetTags,Z_Param_Out_Location,Z_Param_Out_Normal,Z_Param_Out_Instigator,Z_Param_Out_EffectCauser,Z_Param_Out_SourceObject,Z_Param_Out_PhysicalMaterial,Z_Param_Out_GameplayEffectLevel,Z_Param_Out_AbilityLevel,Z_Param_Out_TargetAttachComponent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeGameplayCueParameters) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_NormalizedMagnitude); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_RawMagnitude); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_GET_STRUCT(FGameplayTag,Z_Param_MatchedTagName); \
		P_GET_STRUCT(FGameplayTag,Z_Param_OriginalTag); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_AggregatedSourceTags); \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_AggregatedTargetTags); \
		P_GET_STRUCT(FVector,Z_Param_Location); \
		P_GET_STRUCT(FVector,Z_Param_Normal); \
		P_GET_OBJECT(AActor,Z_Param_Instigator); \
		P_GET_OBJECT(AActor,Z_Param_EffectCauser); \
		P_GET_OBJECT(UObject,Z_Param_SourceObject); \
		P_GET_OBJECT(UPhysicalMaterial,Z_Param_PhysicalMaterial); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameplayEffectLevel); \
		P_GET_PROPERTY(UIntProperty,Z_Param_AbilityLevel); \
		P_GET_OBJECT(USceneComponent,Z_Param_TargetAttachComponent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayCueParameters*)Z_Param__Result=UAbilitySystemBlueprintLibrary::MakeGameplayCueParameters(Z_Param_NormalizedMagnitude,Z_Param_RawMagnitude,Z_Param_EffectContext,Z_Param_MatchedTagName,Z_Param_OriginalTag,Z_Param_AggregatedSourceTags,Z_Param_AggregatedTargetTags,Z_Param_Location,Z_Param_Normal,Z_Param_Instigator,Z_Param_EffectCauser,Z_Param_SourceObject,Z_Param_PhysicalMaterial,Z_Param_GameplayEffectLevel,Z_Param_AbilityLevel,Z_Param_TargetAttachComponent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execDoesGameplayCueMeetTagRequirements) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_STRUCT_REF(FGameplayTagRequirements,Z_Param_Out_SourceTagReqs); \
		P_GET_STRUCT_REF(FGameplayTagRequirements,Z_Param_Out_TargetTagReqs); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::DoesGameplayCueMeetTagRequirements(Z_Param_Parameters,Z_Param_Out_SourceTagReqs,Z_Param_Out_TargetTagReqs); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayCueDirection) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_TargetActor); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Direction); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetGameplayCueDirection(Z_Param_TargetActor,Z_Param_Parameters,Z_Param_Out_Direction); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayCueEndLocationAndNormal) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_TargetActor); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Location); \
		P_GET_STRUCT_REF(FVector,Z_Param_Out_Normal); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetGameplayCueEndLocationAndNormal(Z_Param_TargetActor,Z_Param_Parameters,Z_Param_Out_Location,Z_Param_Out_Normal); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetOrigin) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FVector*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetOrigin(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetInstigatorTransform) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTransform*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetInstigatorTransform(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetInstigatorActor) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetInstigatorActor(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execForwardGameplayCueToTarget) \
	{ \
		P_GET_TINTERFACE(IGameplayCueInterface,Z_Param_TargetCueInterface); \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::ForwardGameplayCueToTarget(Z_Param_TargetCueInterface,EGameplayCueEvent::Type(Z_Param_EventType),Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execHasHitResult) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::HasHitResult(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetHitResult) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FHitResult*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetHitResult(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorByIndex) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActorByIndex(Z_Param_Parameters,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorCount) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActorCount(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsInstigatorLocallyControlledPlayer) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::IsInstigatorLocallyControlledPlayer(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsInstigatorLocallyControlled) \
	{ \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::IsInstigatorLocallyControlled(Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetSourceObject) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UObject**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetSourceObject(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetEffectCauser) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetEffectCauser(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetOriginalInstigatorActor) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetOriginalInstigatorActor(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetInstigatorActor) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetInstigatorActor(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextSetOrigin) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_GET_STRUCT(FVector,Z_Param_Origin); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::EffectContextSetOrigin(Z_Param_EffectContext,Z_Param_Origin); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetOrigin) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FVector*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetOrigin(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextAddHitResult) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_GET_STRUCT(FHitResult,Z_Param_HitResult); \
		P_GET_UBOOL(Z_Param_bReset); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::EffectContextAddHitResult(Z_Param_EffectContext,Z_Param_HitResult,Z_Param_bReset); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextHasHitResult) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextHasHitResult(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextGetHitResult) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FHitResult*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextGetHitResult(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextIsInstigatorLocallyControlled) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextIsInstigatorLocallyControlled(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEffectContextIsValid) \
	{ \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EffectContextIsValid(Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTargetDataEndPointTransform) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTransform*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetTargetDataEndPointTransform(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTargetDataEndPoint) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FVector*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetTargetDataEndPoint(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasEndPoint) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasEndPoint(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTargetDataOrigin) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTransform*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetTargetDataOrigin(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasOrigin) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasOrigin(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetHitResultFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_HitResult); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FHitResult*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Z_Param_Out_HitResult,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasHitResult) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_HitResult); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(Z_Param_Out_HitResult,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetDataHasActor) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::TargetDataHasActor(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execDoesTargetDataContainActor) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::DoesTargetDataContainActor(Z_Param_Out_TargetData,Z_Param_Index,Z_Param_Actor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAllActorsFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<AActor*>*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(Z_Param_Out_TargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorsFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Index); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<AActor*>*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(Z_Param_Out_TargetData,Z_Param_Index); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execCloneSpecHandle) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_InNewInstigator); \
		P_GET_OBJECT(AActor,Z_Param_InEffectCauser); \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_GameplayEffectSpecHandle_Clone); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::CloneSpecHandle(Z_Param_InNewInstigator,Z_Param_InEffectCauser,Z_Param_GameplayEffectSpecHandle_Clone); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeSpecHandle) \
	{ \
		P_GET_OBJECT(UGameplayEffect,Z_Param_InGameplayEffect); \
		P_GET_OBJECT(AActor,Z_Param_InInstigator); \
		P_GET_OBJECT(AActor,Z_Param_InEffectCauser); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_InLevel); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::MakeSpecHandle(Z_Param_InGameplayEffect,Z_Param_InInstigator,Z_Param_InEffectCauser,Z_Param_InLevel); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeFilterHandle) \
	{ \
		P_GET_STRUCT(FGameplayTargetDataFilter,Z_Param_Filter); \
		P_GET_OBJECT(AActor,Z_Param_FilterActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayTargetDataFilterHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::MakeFilterHandle(Z_Param_Filter,Z_Param_FilterActor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execFilterTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetDataHandle); \
		P_GET_STRUCT(FGameplayTargetDataFilterHandle,Z_Param_ActorFilterClass); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::FilterTargetData(Z_Param_Out_TargetDataHandle,Z_Param_ActorFilterClass); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromActorArray) \
	{ \
		P_GET_TARRAY_REF(AActor*,Z_Param_Out_ActorArray); \
		P_GET_UBOOL(Z_Param_OneTargetPerHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(Z_Param_Out_ActorArray,Z_Param_OneTargetPerHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromActor) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Z_Param_Actor); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetDataCountFromTargetData) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_TargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(Z_Param_Out_TargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromHitResult) \
	{ \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_HitResult); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(Z_Param_Out_HitResult); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAbilityTargetDataFromLocations) \
	{ \
		P_GET_STRUCT_REF(FGameplayAbilityTargetingLocationInfo,Z_Param_Out_SourceLocation); \
		P_GET_STRUCT_REF(FGameplayAbilityTargetingLocationInfo,Z_Param_Out_TargetLocation); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(Z_Param_Out_SourceLocation,Z_Param_Out_TargetLocation); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execAppendTargetDataHandle) \
	{ \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetHandle); \
		P_GET_STRUCT_REF(FGameplayAbilityTargetDataHandle,Z_Param_Out_HandleToAdd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetDataHandle*)Z_Param__Result=UAbilitySystemBlueprintLibrary::AppendTargetDataHandle(Z_Param_TargetHandle,Z_Param_Out_HandleToAdd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNotEqual_GameplayAttributeGameplayAttribute) \
	{ \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeA); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeB); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::NotEqual_GameplayAttributeGameplayAttribute(Z_Param_AttributeA,Z_Param_AttributeB); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEqualEqual_GameplayAttributeGameplayAttribute) \
	{ \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeA); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_AttributeB); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EqualEqual_GameplayAttributeGameplayAttribute(Z_Param_AttributeA,Z_Param_AttributeB); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEvaluateAttributeValueWithTagsAndBase) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystem); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_SourceTags); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_TargetTags); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_BaseValue); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccess); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTagsAndBase(Z_Param_AbilitySystem,Z_Param_Attribute,Z_Param_Out_SourceTags,Z_Param_Out_TargetTags,Z_Param_BaseValue,Z_Param_Out_bSuccess); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEvaluateAttributeValueWithTags) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystem); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_SourceTags); \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_TargetTags); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccess); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTags(Z_Param_AbilitySystem,Z_Param_Attribute,Z_Param_Out_SourceTags,Z_Param_Out_TargetTags,Z_Param_Out_bSuccess); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttributeBaseFromAbilitySystemComponent) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystemComponent); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttributeBaseFromAbilitySystemComponent(Z_Param_AbilitySystemComponent,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttributeBase) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttributeBase(Z_Param_Actor,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttributeFromAbilitySystemComponent) \
	{ \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_AbilitySystem); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(Z_Param_AbilitySystem,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetFloatAttribute) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_GET_UBOOL_REF(Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetFloatAttribute(Z_Param_Actor,Z_Param_Attribute,Z_Param_Out_bSuccessfullyFoundAttribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsValid) \
	{ \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UAbilitySystemBlueprintLibrary::IsValid(Z_Param_Attribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSendGameplayEventToActor) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_GET_STRUCT(FGameplayTag,Z_Param_EventTag); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_Payload); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Z_Param_Actor,Z_Param_EventTag,Z_Param_Payload); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAbilitySystemComponent) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_Actor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilitySystemComponent**)Z_Param__Result=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Z_Param_Actor); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilitySystemBlueprintLibrary(); \
	friend struct Z_Construct_UClass_UAbilitySystemBlueprintLibrary_Statics; \
public: \
	DECLARE_CLASS(UAbilitySystemBlueprintLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilitySystemBlueprintLibrary)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_INCLASS \
private: \
	static void StaticRegisterNativesUAbilitySystemBlueprintLibrary(); \
	friend struct Z_Construct_UClass_UAbilitySystemBlueprintLibrary_Statics; \
public: \
	DECLARE_CLASS(UAbilitySystemBlueprintLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilitySystemBlueprintLibrary)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilitySystemBlueprintLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilitySystemBlueprintLibrary) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilitySystemBlueprintLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilitySystemBlueprintLibrary); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilitySystemBlueprintLibrary(UAbilitySystemBlueprintLibrary&&); \
	NO_API UAbilitySystemBlueprintLibrary(const UAbilitySystemBlueprintLibrary&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilitySystemBlueprintLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilitySystemBlueprintLibrary(UAbilitySystemBlueprintLibrary&&); \
	NO_API UAbilitySystemBlueprintLibrary(const UAbilitySystemBlueprintLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilitySystemBlueprintLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilitySystemBlueprintLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilitySystemBlueprintLibrary)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_23_PROLOG
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h_26_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilitySystemBlueprintLibrary."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilitySystemBlueprintLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemBlueprintLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
