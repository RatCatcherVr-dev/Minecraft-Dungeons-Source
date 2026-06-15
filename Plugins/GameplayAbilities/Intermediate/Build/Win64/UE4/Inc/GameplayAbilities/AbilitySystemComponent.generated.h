// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UAnimMontage;
struct FGameplayAbilitySpecHandle;
struct FPredictionKey;
struct FGameplayEventData;
struct FGameplayAbilityActivationInfo;
struct FGameplayAbilityTargetDataHandle;
struct FGameplayTag;
struct FVector_NetQuantize100;
struct FServerAbilityRPCBatch;
class UGameplayAbility;
struct FGameplayTagContainer;
struct FGameplayCueParameters;
struct FGameplayEffectSpecForRPC;
struct FGameplayEffectContextHandle;
struct FGameplayEffectQuery;
struct FActiveGameplayEffectHandle;
class UGameplayEffect;
class UAbilitySystemComponent;
struct FGameplayAttribute;
struct FGameplayEffectSpecHandle;
class UAttributeSet;
class UDataTable;
#ifdef GAMEPLAYABILITIES_AbilitySystemComponent_generated_h
#error "AbilitySystemComponent.generated.h already included, missing '#pragma once' in AbilitySystemComponent.h"
#endif
#define GAMEPLAYABILITIES_AbilitySystemComponent_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_94_DELEGATE \
static inline void FAbilityConfirmOrCancel_DelegateWrapper(const FMulticastScriptDelegate& AbilityConfirmOrCancel) \
{ \
	AbilityConfirmOrCancel.ProcessMulticastDelegate<UObject>(NULL); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_91_DELEGATE \
struct AbilitySystemComponent_eventAbilityAbilityKey_Parms \
{ \
	int32 InputID; \
}; \
static inline void FAbilityAbilityKey_DelegateWrapper(const FMulticastScriptDelegate& AbilityAbilityKey, int32 InputID) \
{ \
	AbilitySystemComponent_eventAbilityAbilityKey_Parms Parms; \
	Parms.InputID=InputID; \
	AbilityAbilityKey.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_RPC_WRAPPERS \
	virtual bool ServerCurrentMontageSetPlayRate_Validate(UAnimMontage* , float ); \
	virtual void ServerCurrentMontageSetPlayRate_Implementation(UAnimMontage* ClientAnimMontage, float InPlayRate); \
	virtual bool ServerCurrentMontageJumpToSectionName_Validate(UAnimMontage* , FName ); \
	virtual void ServerCurrentMontageJumpToSectionName_Implementation(UAnimMontage* ClientAnimMontage, FName SectionName); \
	virtual bool ServerCurrentMontageSetNextSectionName_Validate(UAnimMontage* , float , FName , FName ); \
	virtual void ServerCurrentMontageSetNextSectionName_Implementation(UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName); \
	virtual void ClientActivateAbilitySucceedWithEventData_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData); \
	virtual void ClientActivateAbilitySucceed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey PredictionKey); \
	virtual void ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey); \
	virtual void ClientCancelAbility_Implementation(FGameplayAbilitySpecHandle AbilityToCancel, FGameplayAbilityActivationInfo ActivationInfo); \
	virtual bool ServerCancelAbility_Validate(FGameplayAbilitySpecHandle , FGameplayAbilityActivationInfo ); \
	virtual void ServerCancelAbility_Implementation(FGameplayAbilitySpecHandle AbilityToCancel, FGameplayAbilityActivationInfo ActivationInfo); \
	virtual void ClientEndAbility_Implementation(FGameplayAbilitySpecHandle AbilityToEnd, FGameplayAbilityActivationInfo ActivationInfo); \
	virtual bool ServerEndAbility_Validate(FGameplayAbilitySpecHandle , FGameplayAbilityActivationInfo , FPredictionKey ); \
	virtual void ServerEndAbility_Implementation(FGameplayAbilitySpecHandle AbilityToEnd, FGameplayAbilityActivationInfo ActivationInfo, FPredictionKey PredictionKey); \
	virtual void ClientTryActivateAbility_Implementation(FGameplayAbilitySpecHandle AbilityToActivate); \
	virtual bool ServerTryActivateAbilityWithEventData_Validate(FGameplayAbilitySpecHandle , bool , FPredictionKey , FGameplayEventData ); \
	virtual void ServerTryActivateAbilityWithEventData_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, bool InputPressed, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData); \
	virtual bool ServerTryActivateAbility_Validate(FGameplayAbilitySpecHandle , bool , FPredictionKey ); \
	virtual void ServerTryActivateAbility_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, bool InputPressed, FPredictionKey PredictionKey); \
	virtual bool ServerSetInputReleased_Validate(FGameplayAbilitySpecHandle ); \
	virtual void ServerSetInputReleased_Implementation(FGameplayAbilitySpecHandle AbilityHandle); \
	virtual bool ServerSetInputPressed_Validate(FGameplayAbilitySpecHandle ); \
	virtual void ServerSetInputPressed_Implementation(FGameplayAbilitySpecHandle AbilityHandle); \
	virtual bool ServerSetReplicatedTargetDataCancelled_Validate(FGameplayAbilitySpecHandle , FPredictionKey , FPredictionKey ); \
	virtual void ServerSetReplicatedTargetDataCancelled_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FPredictionKey CurrentPredictionKey); \
	virtual bool ServerSetReplicatedTargetData_Validate(FGameplayAbilitySpecHandle , FPredictionKey , FGameplayAbilityTargetDataHandle const& , FGameplayTag , FPredictionKey ); \
	virtual void ServerSetReplicatedTargetData_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FGameplayAbilityTargetDataHandle const& ReplicatedTargetDataHandle, FGameplayTag ApplicationTag, FPredictionKey CurrentPredictionKey); \
	virtual void ClientSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey); \
	virtual bool ServerSetReplicatedEventWithPayload_Validate(EAbilityGenericReplicatedEvent::Type , FGameplayAbilitySpecHandle , FPredictionKey , FPredictionKey , FVector_NetQuantize100 ); \
	virtual void ServerSetReplicatedEventWithPayload_Implementation(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FPredictionKey CurrentPredictionKey, FVector_NetQuantize100 VectorPayload); \
	virtual bool ServerSetReplicatedEvent_Validate(EAbilityGenericReplicatedEvent::Type , FGameplayAbilitySpecHandle , FPredictionKey , FPredictionKey ); \
	virtual void ServerSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FPredictionKey CurrentPredictionKey); \
	virtual bool ServerAbilityRPCBatch_Validate(FServerAbilityRPCBatch ); \
	virtual void ServerAbilityRPCBatch_Implementation(FServerAbilityRPCBatch BatchInfo); \
	virtual void ClientPrintDebug_Response_Implementation(TArray<FString> const& Strings, int32 GameFlags); \
	virtual bool ServerPrintDebug_RequestWithStrings_Validate(TArray<FString> const& ); \
	virtual void ServerPrintDebug_RequestWithStrings_Implementation(TArray<FString> const& Strings); \
	virtual bool ServerPrintDebug_Request_Validate(); \
	virtual void ServerPrintDebug_Request_Implementation(); \
	virtual void NetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams_Implementation(const FGameplayTagContainer GameplayCueTags, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCueAddedAndWhileActive_FromSpec_Implementation(FGameplayEffectSpecForRPC const& Spec, FPredictionKey PredictionKey); \
	virtual void NetMulticast_InvokeGameplayCueAdded_WithParams_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters Parameters); \
	virtual void NetMulticast_InvokeGameplayCueAdded_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext); \
	virtual void NetMulticast_InvokeGameplayCuesExecuted_WithParams_Implementation(const FGameplayTagContainer GameplayCueTags, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCueExecuted_WithParams_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCuesExecuted_Implementation(const FGameplayTagContainer GameplayCueTags, FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext); \
	virtual void NetMulticast_InvokeGameplayCueExecuted_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext); \
	virtual void NetMulticast_InvokeGameplayCueExecuted_FromSpec_Implementation(const FGameplayEffectSpecForRPC Spec, FPredictionKey PredictionKey); \
 \
	DECLARE_FUNCTION(execServerCurrentMontageSetPlayRate) \
	{ \
		P_GET_OBJECT(UAnimMontage,Z_Param_ClientAnimMontage); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_InPlayRate); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCurrentMontageSetPlayRate_Validate(Z_Param_ClientAnimMontage,Z_Param_InPlayRate)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCurrentMontageSetPlayRate_Validate")); \
			return; \
		} \
		P_THIS->ServerCurrentMontageSetPlayRate_Implementation(Z_Param_ClientAnimMontage,Z_Param_InPlayRate); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerCurrentMontageJumpToSectionName) \
	{ \
		P_GET_OBJECT(UAnimMontage,Z_Param_ClientAnimMontage); \
		P_GET_PROPERTY(UNameProperty,Z_Param_SectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCurrentMontageJumpToSectionName_Validate(Z_Param_ClientAnimMontage,Z_Param_SectionName)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCurrentMontageJumpToSectionName_Validate")); \
			return; \
		} \
		P_THIS->ServerCurrentMontageJumpToSectionName_Implementation(Z_Param_ClientAnimMontage,Z_Param_SectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerCurrentMontageSetNextSectionName) \
	{ \
		P_GET_OBJECT(UAnimMontage,Z_Param_ClientAnimMontage); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClientPosition); \
		P_GET_PROPERTY(UNameProperty,Z_Param_SectionName); \
		P_GET_PROPERTY(UNameProperty,Z_Param_NextSectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCurrentMontageSetNextSectionName_Validate(Z_Param_ClientAnimMontage,Z_Param_ClientPosition,Z_Param_SectionName,Z_Param_NextSectionName)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCurrentMontageSetNextSectionName_Validate")); \
			return; \
		} \
		P_THIS->ServerCurrentMontageSetNextSectionName_Implementation(Z_Param_ClientAnimMontage,Z_Param_ClientPosition,Z_Param_SectionName,Z_Param_NextSectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ReplicatedAnimMontage) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ReplicatedAnimMontage(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientActivateAbilitySucceedWithEventData) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_TriggerEventData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientActivateAbilitySucceedWithEventData_Implementation(Z_Param_AbilityToActivate,Z_Param_PredictionKey,Z_Param_TriggerEventData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientActivateAbilitySucceed) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientActivateAbilitySucceed_Implementation(Z_Param_AbilityToActivate,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientActivateAbilityFailed) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_PROPERTY(UInt16Property,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientActivateAbilityFailed_Implementation(Z_Param_AbilityToActivate,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientCancelAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToCancel); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientCancelAbility_Implementation(Z_Param_AbilityToCancel,Z_Param_ActivationInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerCancelAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToCancel); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCancelAbility_Validate(Z_Param_AbilityToCancel,Z_Param_ActivationInfo)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCancelAbility_Validate")); \
			return; \
		} \
		P_THIS->ServerCancelAbility_Implementation(Z_Param_AbilityToCancel,Z_Param_ActivationInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientEndAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToEnd); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientEndAbility_Implementation(Z_Param_AbilityToEnd,Z_Param_ActivationInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerEndAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToEnd); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerEndAbility_Validate(Z_Param_AbilityToEnd,Z_Param_ActivationInfo,Z_Param_PredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerEndAbility_Validate")); \
			return; \
		} \
		P_THIS->ServerEndAbility_Implementation(Z_Param_AbilityToEnd,Z_Param_ActivationInfo,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientTryActivateAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientTryActivateAbility_Implementation(Z_Param_AbilityToActivate); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerTryActivateAbilityWithEventData) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_UBOOL(Z_Param_InputPressed); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_TriggerEventData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerTryActivateAbilityWithEventData_Validate(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey,Z_Param_TriggerEventData)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerTryActivateAbilityWithEventData_Validate")); \
			return; \
		} \
		P_THIS->ServerTryActivateAbilityWithEventData_Implementation(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey,Z_Param_TriggerEventData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerTryActivateAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_UBOOL(Z_Param_InputPressed); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerTryActivateAbility_Validate(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerTryActivateAbility_Validate")); \
			return; \
		} \
		P_THIS->ServerTryActivateAbility_Implementation(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ActivateAbilities) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ActivateAbilities(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetInputReleased) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetInputReleased_Validate(Z_Param_AbilityHandle)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetInputReleased_Validate")); \
			return; \
		} \
		P_THIS->ServerSetInputReleased_Implementation(Z_Param_AbilityHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetInputPressed) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetInputPressed_Validate(Z_Param_AbilityHandle)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetInputPressed_Validate")); \
			return; \
		} \
		P_THIS->ServerSetInputPressed_Implementation(Z_Param_AbilityHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedTargetDataCancelled) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedTargetDataCancelled_Validate(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedTargetDataCancelled_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedTargetDataCancelled_Implementation(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedTargetData) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_ReplicatedTargetDataHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_ApplicationTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedTargetData_Validate(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_ReplicatedTargetDataHandle,Z_Param_ApplicationTag,Z_Param_CurrentPredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedTargetData_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedTargetData_Implementation(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_ReplicatedTargetDataHandle,Z_Param_ApplicationTag,Z_Param_CurrentPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientSetReplicatedEvent) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedEventWithPayload) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_GET_STRUCT(FVector_NetQuantize100,Z_Param_VectorPayload); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedEventWithPayload_Validate(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey,Z_Param_VectorPayload)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedEventWithPayload_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedEventWithPayload_Implementation(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey,Z_Param_VectorPayload); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedEvent) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedEvent_Validate(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedEvent_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_OwningActor) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_OwningActor(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetCancel) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->TargetCancel(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetConfirm) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->TargetConfirm(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetUserAbilityActivationInhibited) \
	{ \
		P_GET_UBOOL(Z_Param_NewInhibit); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetUserAbilityActivationInhibited(Z_Param_NewInhibit); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetUserAbilityActivationInhibited) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->GetUserAbilityActivationInhibited(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerAbilityRPCBatch) \
	{ \
		P_GET_STRUCT(FServerAbilityRPCBatch,Z_Param_BatchInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerAbilityRPCBatch_Validate(Z_Param_BatchInfo)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerAbilityRPCBatch_Validate")); \
			return; \
		} \
		P_THIS->ServerAbilityRPCBatch_Implementation(Z_Param_BatchInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ServerDebugString) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ServerDebugString(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ClientDebugString) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ClientDebugString(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientPrintDebug_Response) \
	{ \
		P_GET_TARRAY(FString,Z_Param_Strings); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameFlags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientPrintDebug_Response_Implementation(Z_Param_Strings,Z_Param_GameFlags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerPrintDebug_RequestWithStrings) \
	{ \
		P_GET_TARRAY(FString,Z_Param_Strings); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerPrintDebug_RequestWithStrings_Validate(Z_Param_Strings)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerPrintDebug_RequestWithStrings_Validate")); \
			return; \
		} \
		P_THIS->ServerPrintDebug_RequestWithStrings_Implementation(Z_Param_Strings); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerPrintDebug_Request) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerPrintDebug_Request_Validate()) \
		{ \
			RPC_ValidateFailed(TEXT("ServerPrintDebug_Request_Validate")); \
			return; \
		} \
		P_THIS->ServerPrintDebug_Request_Implementation(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTryActivateAbilityByClass) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_InAbilityToActivate); \
		P_GET_UBOOL(Z_Param_bAllowRemoteActivation); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->TryActivateAbilityByClass(Z_Param_InAbilityToActivate,Z_Param_bAllowRemoteActivation); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTryActivateAbilitiesByTag) \
	{ \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_GameplayTagContainer); \
		P_GET_UBOOL(Z_Param_bAllowRemoteActivation); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->TryActivateAbilitiesByTag(Z_Param_Out_GameplayTagContainer,Z_Param_bAllowRemoteActivation); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsGameplayCueActive) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsGameplayCueActive(Z_Param_GameplayCueTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_GameplayCueTags); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams_Implementation(Z_Param_GameplayCueTags,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAddedAndWhileActive_FromSpec) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecForRPC,Z_Param_Spec); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAddedAndWhileActive_FromSpec_Implementation(Z_Param_Spec,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAdded_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAdded_WithParams_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAdded) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAdded_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCuesExecuted_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_GameplayCueTags); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCuesExecuted_WithParams_Implementation(Z_Param_GameplayCueTags,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueExecuted_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueExecuted_WithParams_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCuesExecuted) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_GameplayCueTags); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCuesExecuted_Implementation(Z_Param_GameplayCueTags,Z_Param_PredictionKey,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueExecuted) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueExecuted_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueExecuted_FromSpec) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecForRPC,Z_Param_Spec); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueExecuted_FromSpec_Implementation(Z_Param_Spec,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithGrantedTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithGrantedTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithAppliedTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithAppliedTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithSourceTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithSourceTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveEffects) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectQuery,Z_Param_Out_Query); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FActiveGameplayEffectHandle>*)Z_Param__Result=P_THIS->GetActiveEffects(Z_Param_Out_Query); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToSelf) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToSelf(Z_Param_GameplayEffectClass,Z_Param_Level,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToTarget) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_Target); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToTarget(Z_Param_GameplayEffectClass,Z_Param_Target,Z_Param_Level,Z_Param_Context); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayEffectMagnitude) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->GetGameplayEffectMagnitude(Z_Param_Handle,Z_Param_Attribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetActiveGameplayEffectLevelUsingQuery) \
	{ \
		P_GET_STRUCT(FGameplayEffectQuery,Z_Param_Query); \
		P_GET_PROPERTY(UIntProperty,Z_Param_NewLevel); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetActiveGameplayEffectLevelUsingQuery(Z_Param_Query,Z_Param_NewLevel); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetActiveGameplayEffectLevel) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_NewLevel); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetActiveGameplayEffectLevel(Z_Param_ActiveHandle,Z_Param_NewLevel); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayEffectCount) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_SourceGameplayEffect); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_OptionalInstigatorFilterComponent); \
		P_GET_UBOOL(Z_Param_bEnforceOnGoingCheck); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->GetGameplayEffectCount(Z_Param_SourceGameplayEffect,Z_Param_OptionalInstigatorFilterComponent,Z_Param_bEnforceOnGoingCheck); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeEffectContext) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=P_THIS->MakeEffectContext(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeOutgoingSpec) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=P_THIS->MakeOutgoingSpec(Z_Param_GameplayEffectClass,Z_Param_Level,Z_Param_Context); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveGameplayEffectBySourceEffect) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffect); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_InstigatorAbilitySystemComponent); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->RemoveActiveGameplayEffectBySourceEffect(Z_Param_GameplayEffect,Z_Param_InstigatorAbilitySystemComponent,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveGameplayEffect) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->RemoveActiveGameplayEffect(Z_Param_Handle,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectSpecToSelf) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectSpecHandle,Z_Param_Out_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectSpecToSelf(Z_Param_Out_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectSpecToTarget) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectSpecHandle,Z_Param_Out_SpecHandle); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_Target); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectSpecToTarget(Z_Param_Out_SpecHandle,Z_Param_Target); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_InitStats) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_Attributes); \
		P_GET_OBJECT(UDataTable,Z_Param_DataTable); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_InitStats(Z_Param_Attributes,Z_Param_DataTable); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual bool ServerCurrentMontageSetPlayRate_Validate(UAnimMontage* , float ); \
	virtual void ServerCurrentMontageSetPlayRate_Implementation(UAnimMontage* ClientAnimMontage, float InPlayRate); \
	virtual bool ServerCurrentMontageJumpToSectionName_Validate(UAnimMontage* , FName ); \
	virtual void ServerCurrentMontageJumpToSectionName_Implementation(UAnimMontage* ClientAnimMontage, FName SectionName); \
	virtual bool ServerCurrentMontageSetNextSectionName_Validate(UAnimMontage* , float , FName , FName ); \
	virtual void ServerCurrentMontageSetNextSectionName_Implementation(UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName); \
	virtual void ClientActivateAbilitySucceedWithEventData_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData); \
	virtual void ClientActivateAbilitySucceed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey PredictionKey); \
	virtual void ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey); \
	virtual void ClientCancelAbility_Implementation(FGameplayAbilitySpecHandle AbilityToCancel, FGameplayAbilityActivationInfo ActivationInfo); \
	virtual bool ServerCancelAbility_Validate(FGameplayAbilitySpecHandle , FGameplayAbilityActivationInfo ); \
	virtual void ServerCancelAbility_Implementation(FGameplayAbilitySpecHandle AbilityToCancel, FGameplayAbilityActivationInfo ActivationInfo); \
	virtual void ClientEndAbility_Implementation(FGameplayAbilitySpecHandle AbilityToEnd, FGameplayAbilityActivationInfo ActivationInfo); \
	virtual bool ServerEndAbility_Validate(FGameplayAbilitySpecHandle , FGameplayAbilityActivationInfo , FPredictionKey ); \
	virtual void ServerEndAbility_Implementation(FGameplayAbilitySpecHandle AbilityToEnd, FGameplayAbilityActivationInfo ActivationInfo, FPredictionKey PredictionKey); \
	virtual void ClientTryActivateAbility_Implementation(FGameplayAbilitySpecHandle AbilityToActivate); \
	virtual bool ServerTryActivateAbilityWithEventData_Validate(FGameplayAbilitySpecHandle , bool , FPredictionKey , FGameplayEventData ); \
	virtual void ServerTryActivateAbilityWithEventData_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, bool InputPressed, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData); \
	virtual bool ServerTryActivateAbility_Validate(FGameplayAbilitySpecHandle , bool , FPredictionKey ); \
	virtual void ServerTryActivateAbility_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, bool InputPressed, FPredictionKey PredictionKey); \
	virtual bool ServerSetInputReleased_Validate(FGameplayAbilitySpecHandle ); \
	virtual void ServerSetInputReleased_Implementation(FGameplayAbilitySpecHandle AbilityHandle); \
	virtual bool ServerSetInputPressed_Validate(FGameplayAbilitySpecHandle ); \
	virtual void ServerSetInputPressed_Implementation(FGameplayAbilitySpecHandle AbilityHandle); \
	virtual bool ServerSetReplicatedTargetDataCancelled_Validate(FGameplayAbilitySpecHandle , FPredictionKey , FPredictionKey ); \
	virtual void ServerSetReplicatedTargetDataCancelled_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FPredictionKey CurrentPredictionKey); \
	virtual bool ServerSetReplicatedTargetData_Validate(FGameplayAbilitySpecHandle , FPredictionKey , FGameplayAbilityTargetDataHandle const& , FGameplayTag , FPredictionKey ); \
	virtual void ServerSetReplicatedTargetData_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FGameplayAbilityTargetDataHandle const& ReplicatedTargetDataHandle, FGameplayTag ApplicationTag, FPredictionKey CurrentPredictionKey); \
	virtual void ClientSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey); \
	virtual bool ServerSetReplicatedEventWithPayload_Validate(EAbilityGenericReplicatedEvent::Type , FGameplayAbilitySpecHandle , FPredictionKey , FPredictionKey , FVector_NetQuantize100 ); \
	virtual void ServerSetReplicatedEventWithPayload_Implementation(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FPredictionKey CurrentPredictionKey, FVector_NetQuantize100 VectorPayload); \
	virtual bool ServerSetReplicatedEvent_Validate(EAbilityGenericReplicatedEvent::Type , FGameplayAbilitySpecHandle , FPredictionKey , FPredictionKey ); \
	virtual void ServerSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type EventType, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, FPredictionKey CurrentPredictionKey); \
	virtual bool ServerAbilityRPCBatch_Validate(FServerAbilityRPCBatch ); \
	virtual void ServerAbilityRPCBatch_Implementation(FServerAbilityRPCBatch BatchInfo); \
	virtual void ClientPrintDebug_Response_Implementation(TArray<FString> const& Strings, int32 GameFlags); \
	virtual bool ServerPrintDebug_RequestWithStrings_Validate(TArray<FString> const& ); \
	virtual void ServerPrintDebug_RequestWithStrings_Implementation(TArray<FString> const& Strings); \
	virtual bool ServerPrintDebug_Request_Validate(); \
	virtual void ServerPrintDebug_Request_Implementation(); \
	virtual void NetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams_Implementation(const FGameplayTagContainer GameplayCueTags, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCueAddedAndWhileActive_FromSpec_Implementation(FGameplayEffectSpecForRPC const& Spec, FPredictionKey PredictionKey); \
	virtual void NetMulticast_InvokeGameplayCueAdded_WithParams_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters Parameters); \
	virtual void NetMulticast_InvokeGameplayCueAdded_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext); \
	virtual void NetMulticast_InvokeGameplayCuesExecuted_WithParams_Implementation(const FGameplayTagContainer GameplayCueTags, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCueExecuted_WithParams_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters); \
	virtual void NetMulticast_InvokeGameplayCuesExecuted_Implementation(const FGameplayTagContainer GameplayCueTags, FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext); \
	virtual void NetMulticast_InvokeGameplayCueExecuted_Implementation(const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext); \
	virtual void NetMulticast_InvokeGameplayCueExecuted_FromSpec_Implementation(const FGameplayEffectSpecForRPC Spec, FPredictionKey PredictionKey); \
 \
	DECLARE_FUNCTION(execServerCurrentMontageSetPlayRate) \
	{ \
		P_GET_OBJECT(UAnimMontage,Z_Param_ClientAnimMontage); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_InPlayRate); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCurrentMontageSetPlayRate_Validate(Z_Param_ClientAnimMontage,Z_Param_InPlayRate)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCurrentMontageSetPlayRate_Validate")); \
			return; \
		} \
		P_THIS->ServerCurrentMontageSetPlayRate_Implementation(Z_Param_ClientAnimMontage,Z_Param_InPlayRate); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerCurrentMontageJumpToSectionName) \
	{ \
		P_GET_OBJECT(UAnimMontage,Z_Param_ClientAnimMontage); \
		P_GET_PROPERTY(UNameProperty,Z_Param_SectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCurrentMontageJumpToSectionName_Validate(Z_Param_ClientAnimMontage,Z_Param_SectionName)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCurrentMontageJumpToSectionName_Validate")); \
			return; \
		} \
		P_THIS->ServerCurrentMontageJumpToSectionName_Implementation(Z_Param_ClientAnimMontage,Z_Param_SectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerCurrentMontageSetNextSectionName) \
	{ \
		P_GET_OBJECT(UAnimMontage,Z_Param_ClientAnimMontage); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_ClientPosition); \
		P_GET_PROPERTY(UNameProperty,Z_Param_SectionName); \
		P_GET_PROPERTY(UNameProperty,Z_Param_NextSectionName); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCurrentMontageSetNextSectionName_Validate(Z_Param_ClientAnimMontage,Z_Param_ClientPosition,Z_Param_SectionName,Z_Param_NextSectionName)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCurrentMontageSetNextSectionName_Validate")); \
			return; \
		} \
		P_THIS->ServerCurrentMontageSetNextSectionName_Implementation(Z_Param_ClientAnimMontage,Z_Param_ClientPosition,Z_Param_SectionName,Z_Param_NextSectionName); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ReplicatedAnimMontage) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ReplicatedAnimMontage(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientActivateAbilitySucceedWithEventData) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_TriggerEventData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientActivateAbilitySucceedWithEventData_Implementation(Z_Param_AbilityToActivate,Z_Param_PredictionKey,Z_Param_TriggerEventData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientActivateAbilitySucceed) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientActivateAbilitySucceed_Implementation(Z_Param_AbilityToActivate,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientActivateAbilityFailed) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_PROPERTY(UInt16Property,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientActivateAbilityFailed_Implementation(Z_Param_AbilityToActivate,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientCancelAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToCancel); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientCancelAbility_Implementation(Z_Param_AbilityToCancel,Z_Param_ActivationInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerCancelAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToCancel); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerCancelAbility_Validate(Z_Param_AbilityToCancel,Z_Param_ActivationInfo)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerCancelAbility_Validate")); \
			return; \
		} \
		P_THIS->ServerCancelAbility_Implementation(Z_Param_AbilityToCancel,Z_Param_ActivationInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientEndAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToEnd); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientEndAbility_Implementation(Z_Param_AbilityToEnd,Z_Param_ActivationInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerEndAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToEnd); \
		P_GET_STRUCT(FGameplayAbilityActivationInfo,Z_Param_ActivationInfo); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerEndAbility_Validate(Z_Param_AbilityToEnd,Z_Param_ActivationInfo,Z_Param_PredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerEndAbility_Validate")); \
			return; \
		} \
		P_THIS->ServerEndAbility_Implementation(Z_Param_AbilityToEnd,Z_Param_ActivationInfo,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientTryActivateAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientTryActivateAbility_Implementation(Z_Param_AbilityToActivate); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerTryActivateAbilityWithEventData) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_UBOOL(Z_Param_InputPressed); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEventData,Z_Param_TriggerEventData); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerTryActivateAbilityWithEventData_Validate(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey,Z_Param_TriggerEventData)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerTryActivateAbilityWithEventData_Validate")); \
			return; \
		} \
		P_THIS->ServerTryActivateAbilityWithEventData_Implementation(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey,Z_Param_TriggerEventData); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerTryActivateAbility) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityToActivate); \
		P_GET_UBOOL(Z_Param_InputPressed); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerTryActivateAbility_Validate(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerTryActivateAbility_Validate")); \
			return; \
		} \
		P_THIS->ServerTryActivateAbility_Implementation(Z_Param_AbilityToActivate,Z_Param_InputPressed,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ActivateAbilities) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ActivateAbilities(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetInputReleased) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetInputReleased_Validate(Z_Param_AbilityHandle)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetInputReleased_Validate")); \
			return; \
		} \
		P_THIS->ServerSetInputReleased_Implementation(Z_Param_AbilityHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetInputPressed) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetInputPressed_Validate(Z_Param_AbilityHandle)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetInputPressed_Validate")); \
			return; \
		} \
		P_THIS->ServerSetInputPressed_Implementation(Z_Param_AbilityHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedTargetDataCancelled) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedTargetDataCancelled_Validate(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedTargetDataCancelled_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedTargetDataCancelled_Implementation(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedTargetData) \
	{ \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FGameplayAbilityTargetDataHandle,Z_Param_ReplicatedTargetDataHandle); \
		P_GET_STRUCT(FGameplayTag,Z_Param_ApplicationTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedTargetData_Validate(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_ReplicatedTargetDataHandle,Z_Param_ApplicationTag,Z_Param_CurrentPredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedTargetData_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedTargetData_Implementation(Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_ReplicatedTargetDataHandle,Z_Param_ApplicationTag,Z_Param_CurrentPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientSetReplicatedEvent) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedEventWithPayload) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_GET_STRUCT(FVector_NetQuantize100,Z_Param_VectorPayload); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedEventWithPayload_Validate(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey,Z_Param_VectorPayload)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedEventWithPayload_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedEventWithPayload_Implementation(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey,Z_Param_VectorPayload); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerSetReplicatedEvent) \
	{ \
		P_GET_PROPERTY(UByteProperty,Z_Param_EventType); \
		P_GET_STRUCT(FGameplayAbilitySpecHandle,Z_Param_AbilityHandle); \
		P_GET_STRUCT(FPredictionKey,Z_Param_AbilityOriginalPredictionKey); \
		P_GET_STRUCT(FPredictionKey,Z_Param_CurrentPredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerSetReplicatedEvent_Validate(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerSetReplicatedEvent_Validate")); \
			return; \
		} \
		P_THIS->ServerSetReplicatedEvent_Implementation(EAbilityGenericReplicatedEvent::Type(Z_Param_EventType),Z_Param_AbilityHandle,Z_Param_AbilityOriginalPredictionKey,Z_Param_CurrentPredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_OwningActor) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_OwningActor(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetCancel) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->TargetCancel(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTargetConfirm) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->TargetConfirm(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetUserAbilityActivationInhibited) \
	{ \
		P_GET_UBOOL(Z_Param_NewInhibit); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetUserAbilityActivationInhibited(Z_Param_NewInhibit); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetUserAbilityActivationInhibited) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->GetUserAbilityActivationInhibited(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerAbilityRPCBatch) \
	{ \
		P_GET_STRUCT(FServerAbilityRPCBatch,Z_Param_BatchInfo); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerAbilityRPCBatch_Validate(Z_Param_BatchInfo)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerAbilityRPCBatch_Validate")); \
			return; \
		} \
		P_THIS->ServerAbilityRPCBatch_Implementation(Z_Param_BatchInfo); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ServerDebugString) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ServerDebugString(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnRep_ClientDebugString) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_ClientDebugString(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execClientPrintDebug_Response) \
	{ \
		P_GET_TARRAY(FString,Z_Param_Strings); \
		P_GET_PROPERTY(UIntProperty,Z_Param_GameFlags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ClientPrintDebug_Response_Implementation(Z_Param_Strings,Z_Param_GameFlags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerPrintDebug_RequestWithStrings) \
	{ \
		P_GET_TARRAY(FString,Z_Param_Strings); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerPrintDebug_RequestWithStrings_Validate(Z_Param_Strings)) \
		{ \
			RPC_ValidateFailed(TEXT("ServerPrintDebug_RequestWithStrings_Validate")); \
			return; \
		} \
		P_THIS->ServerPrintDebug_RequestWithStrings_Implementation(Z_Param_Strings); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execServerPrintDebug_Request) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		if (!P_THIS->ServerPrintDebug_Request_Validate()) \
		{ \
			RPC_ValidateFailed(TEXT("ServerPrintDebug_Request_Validate")); \
			return; \
		} \
		P_THIS->ServerPrintDebug_Request_Implementation(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTryActivateAbilityByClass) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_InAbilityToActivate); \
		P_GET_UBOOL(Z_Param_bAllowRemoteActivation); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->TryActivateAbilityByClass(Z_Param_InAbilityToActivate,Z_Param_bAllowRemoteActivation); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execTryActivateAbilitiesByTag) \
	{ \
		P_GET_STRUCT_REF(FGameplayTagContainer,Z_Param_Out_GameplayTagContainer); \
		P_GET_UBOOL(Z_Param_bAllowRemoteActivation); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->TryActivateAbilitiesByTag(Z_Param_Out_GameplayTagContainer,Z_Param_bAllowRemoteActivation); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsGameplayCueActive) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsGameplayCueActive(Z_Param_GameplayCueTag); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_GameplayCueTags); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams_Implementation(Z_Param_GameplayCueTags,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAddedAndWhileActive_FromSpec) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecForRPC,Z_Param_Spec); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAddedAndWhileActive_FromSpec_Implementation(Z_Param_Spec,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAdded_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAdded_WithParams_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueAdded) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueAdded_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCuesExecuted_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_GameplayCueTags); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCuesExecuted_WithParams_Implementation(Z_Param_GameplayCueTags,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueExecuted_WithParams) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayCueParameters,Z_Param_GameplayCueParameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueExecuted_WithParams_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_GameplayCueParameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCuesExecuted) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_GameplayCueTags); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCuesExecuted_Implementation(Z_Param_GameplayCueTags,Z_Param_PredictionKey,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueExecuted) \
	{ \
		P_GET_STRUCT(FGameplayTag,Z_Param_GameplayCueTag); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueExecuted_Implementation(Z_Param_GameplayCueTag,Z_Param_PredictionKey,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execNetMulticast_InvokeGameplayCueExecuted_FromSpec) \
	{ \
		P_GET_STRUCT(FGameplayEffectSpecForRPC,Z_Param_Spec); \
		P_GET_STRUCT(FPredictionKey,Z_Param_PredictionKey); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->NetMulticast_InvokeGameplayCueExecuted_FromSpec_Implementation(Z_Param_Spec,Z_Param_PredictionKey); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithGrantedTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithGrantedTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithAppliedTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithAppliedTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithSourceTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithSourceTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveEffectsWithTags) \
	{ \
		P_GET_STRUCT(FGameplayTagContainer,Z_Param_Tags); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->RemoveActiveEffectsWithTags(Z_Param_Tags); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetActiveEffects) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectQuery,Z_Param_Out_Query); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FActiveGameplayEffectHandle>*)Z_Param__Result=P_THIS->GetActiveEffects(Z_Param_Out_Query); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToSelf) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_EffectContext); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToSelf(Z_Param_GameplayEffectClass,Z_Param_Level,Z_Param_EffectContext); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectToTarget) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_Target); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectToTarget(Z_Param_GameplayEffectClass,Z_Param_Target,Z_Param_Level,Z_Param_Context); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayEffectMagnitude) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_GET_STRUCT(FGameplayAttribute,Z_Param_Attribute); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->GetGameplayEffectMagnitude(Z_Param_Handle,Z_Param_Attribute); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetActiveGameplayEffectLevelUsingQuery) \
	{ \
		P_GET_STRUCT(FGameplayEffectQuery,Z_Param_Query); \
		P_GET_PROPERTY(UIntProperty,Z_Param_NewLevel); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetActiveGameplayEffectLevelUsingQuery(Z_Param_Query,Z_Param_NewLevel); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetActiveGameplayEffectLevel) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_ActiveHandle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_NewLevel); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetActiveGameplayEffectLevel(Z_Param_ActiveHandle,Z_Param_NewLevel); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetGameplayEffectCount) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_SourceGameplayEffect); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_OptionalInstigatorFilterComponent); \
		P_GET_UBOOL(Z_Param_bEnforceOnGoingCheck); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(int32*)Z_Param__Result=P_THIS->GetGameplayEffectCount(Z_Param_SourceGameplayEffect,Z_Param_OptionalInstigatorFilterComponent,Z_Param_bEnforceOnGoingCheck); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeEffectContext) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectContextHandle*)Z_Param__Result=P_THIS->MakeEffectContext(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execMakeOutgoingSpec) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffectClass); \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Level); \
		P_GET_STRUCT(FGameplayEffectContextHandle,Z_Param_Context); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FGameplayEffectSpecHandle*)Z_Param__Result=P_THIS->MakeOutgoingSpec(Z_Param_GameplayEffectClass,Z_Param_Level,Z_Param_Context); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveGameplayEffectBySourceEffect) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_GameplayEffect); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_InstigatorAbilitySystemComponent); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->RemoveActiveGameplayEffectBySourceEffect(Z_Param_GameplayEffect,Z_Param_InstigatorAbilitySystemComponent,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execRemoveActiveGameplayEffect) \
	{ \
		P_GET_STRUCT(FActiveGameplayEffectHandle,Z_Param_Handle); \
		P_GET_PROPERTY(UIntProperty,Z_Param_StacksToRemove); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->RemoveActiveGameplayEffect(Z_Param_Handle,Z_Param_StacksToRemove); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectSpecToSelf) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectSpecHandle,Z_Param_Out_SpecHandle); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectSpecToSelf(Z_Param_Out_SpecHandle); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execBP_ApplyGameplayEffectSpecToTarget) \
	{ \
		P_GET_STRUCT_REF(FGameplayEffectSpecHandle,Z_Param_Out_SpecHandle); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_Target); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FActiveGameplayEffectHandle*)Z_Param__Result=P_THIS->BP_ApplyGameplayEffectSpecToTarget(Z_Param_Out_SpecHandle,Z_Param_Target); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_InitStats) \
	{ \
		P_GET_OBJECT(UClass,Z_Param_Attributes); \
		P_GET_OBJECT(UDataTable,Z_Param_DataTable); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_InitStats(Z_Param_Attributes,Z_Param_DataTable); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_EVENT_PARMS \
	struct AbilitySystemComponent_eventClientActivateAbilityFailed_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToActivate; \
		int16 PredictionKey; \
	}; \
	struct AbilitySystemComponent_eventClientActivateAbilitySucceed_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToActivate; \
		FPredictionKey PredictionKey; \
	}; \
	struct AbilitySystemComponent_eventClientActivateAbilitySucceedWithEventData_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToActivate; \
		FPredictionKey PredictionKey; \
		FGameplayEventData TriggerEventData; \
	}; \
	struct AbilitySystemComponent_eventClientCancelAbility_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToCancel; \
		FGameplayAbilityActivationInfo ActivationInfo; \
	}; \
	struct AbilitySystemComponent_eventClientEndAbility_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToEnd; \
		FGameplayAbilityActivationInfo ActivationInfo; \
	}; \
	struct AbilitySystemComponent_eventClientPrintDebug_Response_Parms \
	{ \
		TArray<FString> Strings; \
		int32 GameFlags; \
	}; \
	struct AbilitySystemComponent_eventClientSetReplicatedEvent_Parms \
	{ \
		TEnumAsByte<EAbilityGenericReplicatedEvent::Type> EventType; \
		FGameplayAbilitySpecHandle AbilityHandle; \
		FPredictionKey AbilityOriginalPredictionKey; \
	}; \
	struct AbilitySystemComponent_eventClientTryActivateAbility_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToActivate; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCueAdded_Parms \
	{ \
		FGameplayTag GameplayCueTag; \
		FPredictionKey PredictionKey; \
		FGameplayEffectContextHandle EffectContext; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCueAdded_WithParams_Parms \
	{ \
		FGameplayTag GameplayCueTag; \
		FPredictionKey PredictionKey; \
		FGameplayCueParameters Parameters; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCueAddedAndWhileActive_FromSpec_Parms \
	{ \
		FGameplayEffectSpecForRPC Spec; \
		FPredictionKey PredictionKey; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams_Parms \
	{ \
		FGameplayTag GameplayCueTag; \
		FPredictionKey PredictionKey; \
		FGameplayCueParameters GameplayCueParameters; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCueExecuted_Parms \
	{ \
		FGameplayTag GameplayCueTag; \
		FPredictionKey PredictionKey; \
		FGameplayEffectContextHandle EffectContext; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCueExecuted_FromSpec_Parms \
	{ \
		FGameplayEffectSpecForRPC Spec; \
		FPredictionKey PredictionKey; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCueExecuted_WithParams_Parms \
	{ \
		FGameplayTag GameplayCueTag; \
		FPredictionKey PredictionKey; \
		FGameplayCueParameters GameplayCueParameters; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams_Parms \
	{ \
		FGameplayTagContainer GameplayCueTags; \
		FPredictionKey PredictionKey; \
		FGameplayCueParameters GameplayCueParameters; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCuesExecuted_Parms \
	{ \
		FGameplayTagContainer GameplayCueTags; \
		FPredictionKey PredictionKey; \
		FGameplayEffectContextHandle EffectContext; \
	}; \
	struct AbilitySystemComponent_eventNetMulticast_InvokeGameplayCuesExecuted_WithParams_Parms \
	{ \
		FGameplayTagContainer GameplayCueTags; \
		FPredictionKey PredictionKey; \
		FGameplayCueParameters GameplayCueParameters; \
	}; \
	struct AbilitySystemComponent_eventServerAbilityRPCBatch_Parms \
	{ \
		FServerAbilityRPCBatch BatchInfo; \
	}; \
	struct AbilitySystemComponent_eventServerCancelAbility_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToCancel; \
		FGameplayAbilityActivationInfo ActivationInfo; \
	}; \
	struct AbilitySystemComponent_eventServerCurrentMontageJumpToSectionName_Parms \
	{ \
		UAnimMontage* ClientAnimMontage; \
		FName SectionName; \
	}; \
	struct AbilitySystemComponent_eventServerCurrentMontageSetNextSectionName_Parms \
	{ \
		UAnimMontage* ClientAnimMontage; \
		float ClientPosition; \
		FName SectionName; \
		FName NextSectionName; \
	}; \
	struct AbilitySystemComponent_eventServerCurrentMontageSetPlayRate_Parms \
	{ \
		UAnimMontage* ClientAnimMontage; \
		float InPlayRate; \
	}; \
	struct AbilitySystemComponent_eventServerEndAbility_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToEnd; \
		FGameplayAbilityActivationInfo ActivationInfo; \
		FPredictionKey PredictionKey; \
	}; \
	struct AbilitySystemComponent_eventServerPrintDebug_RequestWithStrings_Parms \
	{ \
		TArray<FString> Strings; \
	}; \
	struct AbilitySystemComponent_eventServerSetInputPressed_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityHandle; \
	}; \
	struct AbilitySystemComponent_eventServerSetInputReleased_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityHandle; \
	}; \
	struct AbilitySystemComponent_eventServerSetReplicatedEvent_Parms \
	{ \
		TEnumAsByte<EAbilityGenericReplicatedEvent::Type> EventType; \
		FGameplayAbilitySpecHandle AbilityHandle; \
		FPredictionKey AbilityOriginalPredictionKey; \
		FPredictionKey CurrentPredictionKey; \
	}; \
	struct AbilitySystemComponent_eventServerSetReplicatedEventWithPayload_Parms \
	{ \
		TEnumAsByte<EAbilityGenericReplicatedEvent::Type> EventType; \
		FGameplayAbilitySpecHandle AbilityHandle; \
		FPredictionKey AbilityOriginalPredictionKey; \
		FPredictionKey CurrentPredictionKey; \
		FVector_NetQuantize100 VectorPayload; \
	}; \
	struct AbilitySystemComponent_eventServerSetReplicatedTargetData_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityHandle; \
		FPredictionKey AbilityOriginalPredictionKey; \
		FGameplayAbilityTargetDataHandle ReplicatedTargetDataHandle; \
		FGameplayTag ApplicationTag; \
		FPredictionKey CurrentPredictionKey; \
	}; \
	struct AbilitySystemComponent_eventServerSetReplicatedTargetDataCancelled_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityHandle; \
		FPredictionKey AbilityOriginalPredictionKey; \
		FPredictionKey CurrentPredictionKey; \
	}; \
	struct AbilitySystemComponent_eventServerTryActivateAbility_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToActivate; \
		bool InputPressed; \
		FPredictionKey PredictionKey; \
	}; \
	struct AbilitySystemComponent_eventServerTryActivateAbilityWithEventData_Parms \
	{ \
		FGameplayAbilitySpecHandle AbilityToActivate; \
		bool InputPressed; \
		FPredictionKey PredictionKey; \
		FGameplayEventData TriggerEventData; \
	};


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_CALLBACK_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAbilitySystemComponent(); \
	friend struct Z_Construct_UClass_UAbilitySystemComponent_Statics; \
public: \
	DECLARE_CLASS(UAbilitySystemComponent, UGameplayTasksComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilitySystemComponent) \
	virtual UObject* _getUObject() const override { return const_cast<UAbilitySystemComponent*>(this); } \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_INCLASS \
private: \
	static void StaticRegisterNativesUAbilitySystemComponent(); \
	friend struct Z_Construct_UClass_UAbilitySystemComponent_Statics; \
public: \
	DECLARE_CLASS(UAbilitySystemComponent, UGameplayTasksComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UAbilitySystemComponent) \
	virtual UObject* _getUObject() const override { return const_cast<UAbilitySystemComponent*>(this); } \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilitySystemComponent) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilitySystemComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilitySystemComponent); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilitySystemComponent(UAbilitySystemComponent&&); \
	NO_API UAbilitySystemComponent(const UAbilitySystemComponent&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAbilitySystemComponent(UAbilitySystemComponent&&); \
	NO_API UAbilitySystemComponent(const UAbilitySystemComponent&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAbilitySystemComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAbilitySystemComponent); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAbilitySystemComponent)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__ActivatableAbilities() { return STRUCT_OFFSET(UAbilitySystemComponent, ActivatableAbilities); } \
	FORCEINLINE static uint32 __PPO__AllReplicatedInstancedAbilities() { return STRUCT_OFFSET(UAbilitySystemComponent, AllReplicatedInstancedAbilities); } \
	FORCEINLINE static uint32 __PPO__RepAnimMontageInfo() { return STRUCT_OFFSET(UAbilitySystemComponent, RepAnimMontageInfo); } \
	FORCEINLINE static uint32 __PPO__bCachedIsNetSimulated() { return STRUCT_OFFSET(UAbilitySystemComponent, bCachedIsNetSimulated); } \
	FORCEINLINE static uint32 __PPO__bPendingMontageRep() { return STRUCT_OFFSET(UAbilitySystemComponent, bPendingMontageRep); } \
	FORCEINLINE static uint32 __PPO__LocalAnimMontageInfo() { return STRUCT_OFFSET(UAbilitySystemComponent, LocalAnimMontageInfo); } \
	FORCEINLINE static uint32 __PPO__ActiveGameplayEffects() { return STRUCT_OFFSET(UAbilitySystemComponent, ActiveGameplayEffects); } \
	FORCEINLINE static uint32 __PPO__ActiveGameplayCues() { return STRUCT_OFFSET(UAbilitySystemComponent, ActiveGameplayCues); } \
	FORCEINLINE static uint32 __PPO__MinimalReplicationGameplayCues() { return STRUCT_OFFSET(UAbilitySystemComponent, MinimalReplicationGameplayCues); } \
	FORCEINLINE static uint32 __PPO__BlockedAbilityBindings() { return STRUCT_OFFSET(UAbilitySystemComponent, BlockedAbilityBindings); } \
	FORCEINLINE static uint32 __PPO__MinimalReplicationTags() { return STRUCT_OFFSET(UAbilitySystemComponent, MinimalReplicationTags); }


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_85_PROLOG \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_EVENT_PARMS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h_88_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class AbilitySystemComponent."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UAbilitySystemComponent>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_AbilitySystemComponent_h


#define FOREACH_ENUM_EGAMEPLAYEFFECTREPLICATIONMODE(op) \
	op(EGameplayEffectReplicationMode::Minimal) \
	op(EGameplayEffectReplicationMode::Mixed) \
	op(EGameplayEffectReplicationMode::Full) 

enum class EGameplayEffectReplicationMode : uint8;
template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EGameplayEffectReplicationMode>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
