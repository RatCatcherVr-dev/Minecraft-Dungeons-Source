// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayPrediction.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayPrediction() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializer();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializerItem();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FPredictionKey();
	COREUOBJECT_API UClass* Z_Construct_UClass_UPackageMap();
// End Cross Module References
class UScriptStruct* FReplicatedPredictionKeyMap::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ReplicatedPredictionKeyMap"), sizeof(FReplicatedPredictionKeyMap), Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FReplicatedPredictionKeyMap>()
{
	return FReplicatedPredictionKeyMap::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FReplicatedPredictionKeyMap(FReplicatedPredictionKeyMap::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("ReplicatedPredictionKeyMap"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFReplicatedPredictionKeyMap
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFReplicatedPredictionKeyMap()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("ReplicatedPredictionKeyMap")),new UScriptStruct::TCppStructOps<FReplicatedPredictionKeyMap>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFReplicatedPredictionKeyMap;
	struct Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PredictionKeys_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_PredictionKeys;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_PredictionKeys_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FReplicatedPredictionKeyMap>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys = { "PredictionKeys", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FReplicatedPredictionKeyMap, PredictionKeys), METADATA_PARAMS(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys_Inner = { "PredictionKeys", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		Z_Construct_UScriptStruct_FFastArraySerializer,
		&NewStructOps,
		"ReplicatedPredictionKeyMap",
		sizeof(FReplicatedPredictionKeyMap),
		alignof(FReplicatedPredictionKeyMap),
		Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("ReplicatedPredictionKeyMap"), sizeof(FReplicatedPredictionKeyMap), Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Hash() { return 3491969469U; }
class UScriptStruct* FReplicatedPredictionKeyItem::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ReplicatedPredictionKeyItem"), sizeof(FReplicatedPredictionKeyItem), Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FReplicatedPredictionKeyItem>()
{
	return FReplicatedPredictionKeyItem::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FReplicatedPredictionKeyItem(FReplicatedPredictionKeyItem::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("ReplicatedPredictionKeyItem"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFReplicatedPredictionKeyItem
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFReplicatedPredictionKeyItem()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("ReplicatedPredictionKeyItem")),new UScriptStruct::TCppStructOps<FReplicatedPredictionKeyItem>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFReplicatedPredictionKeyItem;
	struct Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PredictionKey_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_PredictionKey;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FReplicatedPredictionKeyItem>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewProp_PredictionKey_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewProp_PredictionKey = { "PredictionKey", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FReplicatedPredictionKeyItem, PredictionKey), Z_Construct_UScriptStruct_FPredictionKey, METADATA_PARAMS(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewProp_PredictionKey_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewProp_PredictionKey_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewProp_PredictionKey,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		Z_Construct_UScriptStruct_FFastArraySerializerItem,
		&NewStructOps,
		"ReplicatedPredictionKeyItem",
		sizeof(FReplicatedPredictionKeyItem),
		alignof(FReplicatedPredictionKeyItem),
		Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("ReplicatedPredictionKeyItem"), sizeof(FReplicatedPredictionKeyItem), Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Hash() { return 2035889836U; }
class UScriptStruct* FPredictionKey::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FPredictionKey_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FPredictionKey, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("PredictionKey"), sizeof(FPredictionKey), Get_Z_Construct_UScriptStruct_FPredictionKey_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FPredictionKey>()
{
	return FPredictionKey::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FPredictionKey(FPredictionKey::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("PredictionKey"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFPredictionKey
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFPredictionKey()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("PredictionKey")),new UScriptStruct::TCppStructOps<FPredictionKey>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFPredictionKey;
	struct Z_Construct_UScriptStruct_FPredictionKey_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bIsServerInitiated_MetaData[];
#endif
		static void NewProp_bIsServerInitiated_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bIsServerInitiated;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bIsStale_MetaData[];
#endif
		static void NewProp_bIsStale_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bIsStale;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PredictiveConnection_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_PredictiveConnection;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Base_MetaData[];
#endif
		static const UE4CodeGen_Private::FInt16PropertyParams NewProp_Base;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Current_MetaData[];
#endif
		static const UE4CodeGen_Private::FInt16PropertyParams NewProp_Current;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPredictionKey_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
		{ "ToolTip", "FPredictionKey is a generic way of supporting Clientside Prediction in the GameplayAbility system.\nA FPredictionKey is essentially an ID for identifying predictive actions and side effects that are\ndone on a client. UAbilitySystemComponent supports synchronization of the prediction key and its side effects\nbetween client and server.\n\nEssentially, anything can be associated with a PredictionKey, for example activating an Ability.\nThe client can generates a fresh PredictionKey and sends it to the server in his ServerTryActivateAbility call.\nThe server can confirm or reject this call (ClientActivateAbilitySucceed/Failed).\n\nWhile the client is predictively his ability, he is creating side effects (GameplayEffects, TriggeredEvents, Animations, etc).\nAs the client predicts these side effects, he associates each one with the prediction key generated at the start of the ability\nactivation.\n\nIf the ability activation is rejected, the client can immediately revert these side effects.\nIf the ability activation is accepted, the client must wait until the replicated side effects are sent to the server.\n        (the ClientActivatbleAbilitySucceed RPC will be immediately sent. Property replication may happen a few frames later).\n        Once replication of the server created side effects is finished, the client can undo his locally predictive side effects.\n\nThe main things FPredictionKey itself provides are:\n        -Unique ID and a system for having dependant chains of Prediction Keys (\"Current\" and \"Base\" integers)\n        -A special implementation of ::NetSerialize *** which only serializes the prediction key to the predicting client ***\n                -This is important as it allows us to serialize prediction keys in replicated state, knowing that only clients that gave the server the prediction key will actually see them!" },
	};
#endif
	void* Z_Construct_UScriptStruct_FPredictionKey_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FPredictionKey>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
		{ "ToolTip", "True if this was created as a server initiated activation key, used to identify server activations but cannot be used for prediction" },
	};
#endif
	void Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated_SetBit(void* Obj)
	{
		((FPredictionKey*)Obj)->bIsServerInitiated = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated = { "bIsServerInitiated", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FPredictionKey), &Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsStale_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
		{ "ToolTip", "If stale, this key cannot be used for more prediction" },
	};
#endif
	void Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsStale_SetBit(void* Obj)
	{
		((FPredictionKey*)Obj)->bIsStale = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsStale = { "bIsStale", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FPredictionKey), &Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsStale_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsStale_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsStale_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_PredictiveConnection_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
		{ "ToolTip", "On the server, what network connection this was serialized on." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_PredictiveConnection = { "PredictiveConnection", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FPredictionKey, PredictiveConnection), Z_Construct_UClass_UPackageMap, METADATA_PARAMS(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_PredictiveConnection_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_PredictiveConnection_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Base_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
		{ "ToolTip", "If non 0, the prediction key this was created from" },
	};
#endif
	const UE4CodeGen_Private::FInt16PropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Base = { "Base", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int16, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FPredictionKey, Base), METADATA_PARAMS(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Base_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Base_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Current_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
		{ "ToolTip", "The unique ID of this prediction key" },
	};
#endif
	const UE4CodeGen_Private::FInt16PropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Current = { "Current", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int16, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FPredictionKey, Current), METADATA_PARAMS(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Current_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Current_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FPredictionKey_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsStale,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_PredictiveConnection,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Base,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Current,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FPredictionKey_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"PredictionKey",
		sizeof(FPredictionKey),
		alignof(FPredictionKey),
		Z_Construct_UScriptStruct_FPredictionKey_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FPredictionKey_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FPredictionKey()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FPredictionKey_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("PredictionKey"), sizeof(FPredictionKey), Get_Z_Construct_UScriptStruct_FPredictionKey_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FPredictionKey_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FPredictionKey_Hash() { return 3129486944U; }
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
