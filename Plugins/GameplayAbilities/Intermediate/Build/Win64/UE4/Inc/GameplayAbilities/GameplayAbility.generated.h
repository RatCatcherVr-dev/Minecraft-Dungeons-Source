// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayAbilityTargetingLocationInfo;
struct FGameplayTag;
struct FGameplayCueParameters;
struct FGameplayEffectContextHandle;
struct FActiveGameplayEffectHandle;
struct FGameplayTagContainer;
struct FGameplayEffectSpecHandle;
struct FGameplayAbilityTargetDataHandle;
class UGameplayEffect;
struct FGameplayEventData;
class UObject;
class UAnimMontage;
class UAbilitySystemComponent;
class USkeletalMeshComponent;
class AActor;
struct FGameplayAbilityActorInfo;
#ifdef GAMEPLAYABILITIES_GameplayAbility_generated_h
#error "GameplayAbility.generated.h already included, missing '#pragma once' in GameplayAbility.h"
#endif
#define GAMEPLAYABILITIES_GameplayAbility_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_77_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FAbilityTriggerData_Statics; \
	GAMEPLAYABILITIES_API static class UScriptStruct* StaticStruct();


template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<struct FAbilityTriggerData>();

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execMakeTargetLocationInfoFromOwnerSkeletalMeshComponent) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_SocketName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetingLocationInfo*)Z_Param__Result=P_THIS->MakeTargetLocationInfoFromOwnerSkeletalMeshComponent(Z_Param_SocketName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeTargetLocationInfoFromOwnerActor) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetingLocationInfo*)Z_Param__Result=P_THIS->MakeTargetLocationInfoFromOwnerActor(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMontageStop) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_OverrideBlendOutTime); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MontageStop(Z_Param_OverrideBlendOutTime); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMontageSetNextSectionName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_FromSectionName); \
		P_GET_PROPERTY(UNameProperty,Z_Param_ToSectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MontageSetNextSectionName(Z_Param_FromSectionName,Z_Param_ToSectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMontageJumpToSection) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_SectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MontageJumpToSection(Z_Param_SectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEndAbilityState) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_OptionalStateNameToEnd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->EndAbilityState(Z_Param_OptionalStateNameToEnd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execCancelTaskByInstanceName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_InstanceName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->CancelTaskByInstanceName(Z_Param_InstanceName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEndTaskByInstanceName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_InstanceName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->EndTaskByInstanceName(Z_Param_InstanceName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execConfirmTaskByInstanceName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_InstanceName); \
		P_GET_UBOOL(Z_Param_bEndTask); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ConfirmTaskByInstanceName(Z_Param_InstanceName,Z_Param_bEndTask); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_RemoveGameplayCue) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_RemoveGameplayCue(Z_Param_GameplayCueTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_AddGameplayCueWithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_GameplayCueParameter); \
		P_GET_UBOOL(Z_Param_bRemoveOnAbilityEnd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_AddGameplayCueWithParams(Z_Param_GameplayCueTag,Z_Param_Out_GameplayCueParameter,Z_Param_bRemoveOnAbilityEnd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_AddGameplayCue) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_GET_UBOOL(Z_Param_bRemoveOnAbilityEnd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_AddGameplayCue(Z_Param_GameplayCueTag,Z_Param_Context,Z_Param_bRemoveOnAbilityEnd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ExecuteGameplayCueWithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_ExecuteGameplayCueWithParams(Z_Param_GameplayCueTag,Z_Param_Out_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ExecuteGameplayCue) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_ExecuteGameplayCue(Z_Param_GameplayCueTag,Z_Param_Context); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_RemoveGameplayEffectFromOwnerWithHandle) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->BP_RemoveGameplayEffectFromOwnerWithHandle(Z_Param_Handle,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_RemoveGameplayEffectFromOwnerWithGrantedTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_WithGrantedTags); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->BP_RemoveGameplayEffectFromOwnerWithGrantedTags(Z_Param_WithGrantedTags,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_RemoveGameplayEffectFromOwnerWithAssetTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_WithAssetTags); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->BP_RemoveGameplayEffectFromOwnerWithAssetTags(Z_Param_WithAssetTags,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ApplyGameplayEffectSpecToTarget) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_EffectSpecHandle); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FActiveGameplayEffectHandle>*)Z_Param__Result=P_THIS->K2_ApplyGameplayEffectSpecToTarget(Z_Param_EffectSpecHandle,Z_Param_TargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToTarget) \
	{ \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameplayEffectLevel); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Stacks); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FActiveGameplayEffectHandle>*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToTarget(Z_Param_TargetData,Z_Param_GameplayEffectClass,Z_Param_GameplayEffectLevel,Z_Param_Stacks); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ApplyGameplayEffectSpecToOwner) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_EffectSpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->K2_ApplyGameplayEffectSpecToOwner(Z_Param_EffectSpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToOwner) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameplayEffectLevel); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Stacks); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToOwner(Z_Param_GameplayEffectClass,Z_Param_GameplayEffectLevel,Z_Param_Stacks); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_EndAbility) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_EndAbility(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSendGameplayEvent) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_EventTag); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_Payload); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SendGameplayEvent(Z_Param_EventTag,Z_Param_Payload); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveGrantedByEffect) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->RemoveGrantedByEffect(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execInvalidateClientPredictionKey) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->InvalidateClientPredictionKey(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCurrentSourceObject) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UObject**)Z_Param__Result=P_THIS->GetCurrentSourceObject(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAbilityLevel) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->GetAbilityLevel(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCurrentMontage) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAnimMontage**)Z_Param__Result=P_THIS->GetCurrentMontage(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CheckAbilityCost) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CheckAbilityCost(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CheckAbilityCooldown) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CheckAbilityCooldown(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CommitAbilityCost) \
	{ \
		P_GET_UBOOL(Z_Param_BroadcastCommitEvent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CommitAbilityCost(Z_Param_BroadcastCommitEvent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CommitAbilityCooldown) \
	{ \
		P_GET_UBOOL(Z_Param_BroadcastCommitEvent); \
		P_GET_UBOOL(Z_Param_ForceCooldown); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CommitAbilityCooldown(Z_Param_BroadcastCommitEvent,Z_Param_ForceCooldown); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CommitAbility) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CommitAbility(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetCanBeCanceled) \
	{ \
		P_GET_UBOOL(Z_Param_bCanBeCanceled); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetCanBeCanceled(Z_Param_bCanBeCanceled); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CancelAbility) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_CancelAbility(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetShouldBlockOtherAbilities) \
	{ \
		P_GET_UBOOL(Z_Param_bShouldBlockAbilities); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetShouldBlockOtherAbilities(Z_Param_bShouldBlockAbilities); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCooldownTimeRemaining) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->GetCooldownTimeRemaining(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeOutgoingGameplayEffectSpec) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=P_THIS->MakeOutgoingGameplayEffectSpec(Z_Param_GameplayEffectClass,Z_Param_Level); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetContextFromOwner) \
	{ \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_OptionalTargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=P_THIS->GetContextFromOwner(Z_Param_OptionalTargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGrantedByEffectContext) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=P_THIS->GetGrantedByEffectContext(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAbilitySystemComponentFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilitySystemComponent**)Z_Param__Result=P_THIS->GetAbilitySystemComponentFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetOwningComponentFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(USkeletalMeshComponent**)Z_Param__Result=P_THIS->GetOwningComponentFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAvatarActorFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=P_THIS->GetAvatarActorFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetOwningActorFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=P_THIS->GetOwningActorFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityActorInfo*)Z_Param__Result=P_THIS->GetActorInfo(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execMakeTargetLocationInfoFromOwnerSkeletalMeshComponent) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_SocketName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetingLocationInfo*)Z_Param__Result=P_THIS->MakeTargetLocationInfoFromOwnerSkeletalMeshComponent(Z_Param_SocketName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeTargetLocationInfoFromOwnerActor) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityTargetingLocationInfo*)Z_Param__Result=P_THIS->MakeTargetLocationInfoFromOwnerActor(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMontageStop) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_OverrideBlendOutTime); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MontageStop(Z_Param_OverrideBlendOutTime); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMontageSetNextSectionName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_FromSectionName); \
		P_GET_PROPERTY(UNameProperty,Z_Param_ToSectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MontageSetNextSectionName(Z_Param_FromSectionName,Z_Param_ToSectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMontageJumpToSection) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_SectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->MontageJumpToSection(Z_Param_SectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEndAbilityState) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_OptionalStateNameToEnd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->EndAbilityState(Z_Param_OptionalStateNameToEnd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execCancelTaskByInstanceName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_InstanceName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->CancelTaskByInstanceName(Z_Param_InstanceName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execEndTaskByInstanceName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_InstanceName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->EndTaskByInstanceName(Z_Param_InstanceName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execConfirmTaskByInstanceName) \
	{ \
		P_GET_PROPERTY(UNameProperty,Z_Param_InstanceName); \
		P_GET_UBOOL(Z_Param_bEndTask); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ConfirmTaskByInstanceName(Z_Param_InstanceName,Z_Param_bEndTask); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_RemoveGameplayCue) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_RemoveGameplayCue(Z_Param_GameplayCueTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_AddGameplayCueWithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_GameplayCueParameter); \
		P_GET_UBOOL(Z_Param_bRemoveOnAbilityEnd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_AddGameplayCueWithParams(Z_Param_GameplayCueTag,Z_Param_Out_GameplayCueParameter,Z_Param_bRemoveOnAbilityEnd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_AddGameplayCue) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_GET_UBOOL(Z_Param_bRemoveOnAbilityEnd); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_AddGameplayCue(Z_Param_GameplayCueTag,Z_Param_Context,Z_Param_bRemoveOnAbilityEnd); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ExecuteGameplayCueWithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_ExecuteGameplayCueWithParams(Z_Param_GameplayCueTag,Z_Param_Out_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ExecuteGameplayCue) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_ExecuteGameplayCue(Z_Param_GameplayCueTag,Z_Param_Context); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_RemoveGameplayEffectFromOwnerWithHandle) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->BP_RemoveGameplayEffectFromOwnerWithHandle(Z_Param_Handle,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_RemoveGameplayEffectFromOwnerWithGrantedTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_WithGrantedTags); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->BP_RemoveGameplayEffectFromOwnerWithGrantedTags(Z_Param_WithGrantedTags,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_RemoveGameplayEffectFromOwnerWithAssetTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_WithAssetTags); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->BP_RemoveGameplayEffectFromOwnerWithAssetTags(Z_Param_WithAssetTags,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ApplyGameplayEffectSpecToTarget) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_EffectSpecHandle); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FActiveGameplayEffectHandle>*)Z_Param__Result=P_THIS->K2_ApplyGameplayEffectSpecToTarget(Z_Param_EffectSpecHandle,Z_Param_TargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToTarget) \
	{ \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_TargetData); \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameplayEffectLevel); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Stacks); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FActiveGameplayEffectHandle>*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToTarget(Z_Param_TargetData,Z_Param_GameplayEffectClass,Z_Param_GameplayEffectLevel,Z_Param_Stacks); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_ApplyGameplayEffectSpecToOwner) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecHandle,Z_Param_EffectSpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->K2_ApplyGameplayEffectSpecToOwner(Z_Param_EffectSpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToOwner) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameplayEffectLevel); \
		P_GET_PROPERTY(UIntProperty,Z_Param_Stacks); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToOwner(Z_Param_GameplayEffectClass,Z_Param_GameplayEffectLevel,Z_Param_Stacks); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_EndAbility) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_EndAbility(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSendGameplayEvent) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_EventTag); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_Payload); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SendGameplayEvent(Z_Param_EventTag,Z_Param_Payload); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveGrantedByEffect) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->RemoveGrantedByEffect(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execInvalidateClientPredictionKey) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->InvalidateClientPredictionKey(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCurrentSourceObject) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UObject**)Z_Param__Result=P_THIS->GetCurrentSourceObject(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAbilityLevel) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->GetAbilityLevel(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCurrentMontage) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAnimMontage**)Z_Param__Result=P_THIS->GetCurrentMontage(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CheckAbilityCost) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CheckAbilityCost(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CheckAbilityCooldown) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CheckAbilityCooldown(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CommitAbilityCost) \
	{ \
		P_GET_UBOOL(Z_Param_BroadcastCommitEvent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CommitAbilityCost(Z_Param_BroadcastCommitEvent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CommitAbilityCooldown) \
	{ \
		P_GET_UBOOL(Z_Param_BroadcastCommitEvent); \
		P_GET_UBOOL(Z_Param_ForceCooldown); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CommitAbilityCooldown(Z_Param_BroadcastCommitEvent,Z_Param_ForceCooldown); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CommitAbility) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->K2_CommitAbility(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetCanBeCanceled) \
	{ \
		P_GET_UBOOL(Z_Param_bCanBeCanceled); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetCanBeCanceled(Z_Param_bCanBeCanceled); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_CancelAbility) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_CancelAbility(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetShouldBlockOtherAbilities) \
	{ \
		P_GET_UBOOL(Z_Param_bShouldBlockAbilities); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetShouldBlockOtherAbilities(Z_Param_bShouldBlockAbilities); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCooldownTimeRemaining) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->GetCooldownTimeRemaining(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeOutgoingGameplayEffectSpec) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=P_THIS->MakeOutgoingGameplayEffectSpec(Z_Param_GameplayEffectClass,Z_Param_Level); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetContextFromOwner) \
	{ \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_OptionalTargetData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=P_THIS->GetContextFromOwner(Z_Param_OptionalTargetData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGrantedByEffectContext) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=P_THIS->GetGrantedByEffectContext(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAbilitySystemComponentFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(UAbilitySystemComponent**)Z_Param__Result=P_THIS->GetAbilitySystemComponentFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetOwningComponentFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(USkeletalMeshComponent**)Z_Param__Result=P_THIS->GetOwningComponentFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetAvatarActorFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=P_THIS->GetAvatarActorFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetOwningActorFromActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(AActor**)Z_Param__Result=P_THIS->GetOwningActorFromActorInfo(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActorInfo) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayAbilityActorInfo*)Z_Param__Result=P_THIS->GetActorInfo(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_EVENT_PARMS \
	struct GameplayAbility_eventK2_ActivateAbilityFromEvent_Parms \
	{ \
		FGameplayEventData EventData; \
	}; \
	struct GameplayAbility_eventK2_CanActivateAbility_Parms \
	{ \
		FGameplayAbilityActorInfo ActorInfo; \
		FGameplayTagContainer RelevantTags; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayAbility_eventK2_CanActivateAbility_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	}; \
	struct GameplayAbility_eventK2_OnEndAbility_Parms \
	{ \
		bool bWasCancelled; \
	}; \
	struct GameplayAbility_eventK2_ShouldAbilityRespondToEvent_Parms \
	{ \
		FGameplayAbilityActorInfo ActorInfo; \
		FGameplayEventData Payload; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayAbility_eventK2_ShouldAbilityRespondToEvent_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	};


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_CALLBACK_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayAbility(); \
	friend struct Z_Construct_UClass_UGameplayAbility_Statics; \
public: \
	DECLARE_CLASS(UGameplayAbility, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayAbility) \
	virtual UObject* _getUObject() const override { return const_cast<UGameplayAbility*>(this); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_INCLASS \
private: \
	static void StaticRegisterNativesUGameplayAbility(); \
	friend struct Z_Construct_UClass_UGameplayAbility_Statics; \
public: \
	DECLARE_CLASS(UGameplayAbility, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayAbility) \
	virtual UObject* _getUObject() const override { return const_cast<UGameplayAbility*>(this); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayAbility) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayAbility); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayAbility); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayAbility(UGameplayAbility&&); \
	NO_API UGameplayAbility(const UGameplayAbility&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayAbility(UGameplayAbility&&); \
	NO_API UGameplayAbility(const UGameplayAbility&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayAbility); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayAbility); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayAbility)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__ReplicationPolicy() { return STRUCT_OFFSET(UGameplayAbility, ReplicationPolicy); } \
	FORCEINLINE static uint32 __PPO__InstancingPolicy() { return STRUCT_OFFSET(UGameplayAbility, InstancingPolicy); } \
	FORCEINLINE static uint32 __PPO__bServerRespectsRemoteAbilityCancellation() { return STRUCT_OFFSET(UGameplayAbility, bServerRespectsRemoteAbilityCancellation); } \
	FORCEINLINE static uint32 __PPO__bRetriggerInstancedAbility() { return STRUCT_OFFSET(UGameplayAbility, bRetriggerInstancedAbility); } \
	FORCEINLINE static uint32 __PPO__CurrentActivationInfo() { return STRUCT_OFFSET(UGameplayAbility, CurrentActivationInfo); } \
	FORCEINLINE static uint32 __PPO__CurrentEventData() { return STRUCT_OFFSET(UGameplayAbility, CurrentEventData); } \
	FORCEINLINE static uint32 __PPO__NetExecutionPolicy() { return STRUCT_OFFSET(UGameplayAbility, NetExecutionPolicy); } \
	FORCEINLINE static uint32 __PPO__CostGameplayEffectClass() { return STRUCT_OFFSET(UGameplayAbility, CostGameplayEffectClass); } \
	FORCEINLINE static uint32 __PPO__AbilityTriggers() { return STRUCT_OFFSET(UGameplayAbility, AbilityTriggers); } \
	FORCEINLINE static uint32 __PPO__CooldownGameplayEffectClass() { return STRUCT_OFFSET(UGameplayAbility, CooldownGameplayEffectClass); } \
	FORCEINLINE static uint32 __PPO__CancelAbilitiesMatchingTagQuery() { return STRUCT_OFFSET(UGameplayAbility, CancelAbilitiesMatchingTagQuery); } \
	FORCEINLINE static uint32 __PPO__CancelAbilitiesWithTag() { return STRUCT_OFFSET(UGameplayAbility, CancelAbilitiesWithTag); } \
	FORCEINLINE static uint32 __PPO__BlockAbilitiesWithTag() { return STRUCT_OFFSET(UGameplayAbility, BlockAbilitiesWithTag); } \
	FORCEINLINE static uint32 __PPO__ActivationOwnedTags() { return STRUCT_OFFSET(UGameplayAbility, ActivationOwnedTags); } \
	FORCEINLINE static uint32 __PPO__ActivationRequiredTags() { return STRUCT_OFFSET(UGameplayAbility, ActivationRequiredTags); } \
	FORCEINLINE static uint32 __PPO__ActivationBlockedTags() { return STRUCT_OFFSET(UGameplayAbility, ActivationBlockedTags); } \
	FORCEINLINE static uint32 __PPO__SourceRequiredTags() { return STRUCT_OFFSET(UGameplayAbility, SourceRequiredTags); } \
	FORCEINLINE static uint32 __PPO__SourceBlockedTags() { return STRUCT_OFFSET(UGameplayAbility, SourceBlockedTags); } \
	FORCEINLINE static uint32 __PPO__TargetRequiredTags() { return STRUCT_OFFSET(UGameplayAbility, TargetRequiredTags); } \
	FORCEINLINE static uint32 __PPO__TargetBlockedTags() { return STRUCT_OFFSET(UGameplayAbility, TargetBlockedTags); } \
	FORCEINLINE static uint32 __PPO__ActiveTasks() { return STRUCT_OFFSET(UGameplayAbility, ActiveTasks); } \
	FORCEINLINE static uint32 __PPO__CurrentMontage() { return STRUCT_OFFSET(UGameplayAbility, CurrentMontage); } \
	FORCEINLINE static uint32 __PPO__bIsActive() { return STRUCT_OFFSET(UGameplayAbility, bIsActive); } \
	FORCEINLINE static uint32 __PPO__bIsCancelable() { return STRUCT_OFFSET(UGameplayAbility, bIsCancelable); } \
	FORCEINLINE static uint32 __PPO__bIsBlockingOtherAbilities() { return STRUCT_OFFSET(UGameplayAbility, bIsBlockingOtherAbilities); } \
	FORCEINLINE static uint32 __PPO__bMarkPendingKillOnAbilityEnd() { return STRUCT_OFFSET(UGameplayAbility, bMarkPendingKillOnAbilityEnd); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_93_PROLOG \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_EVENT_PARMS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h_96_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class GameplayAbility."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UGameplayAbility>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_Abilities_GameplayAbility_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
