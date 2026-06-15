// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCueInterface.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCueInterface() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTag();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FActiveGameplayCueContainer();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializer();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FActiveGameplayCue();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializerItem();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueParameters();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FPredictionKey();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueInterface_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueInterface();
	COREUOBJECT_API UClass* Z_Construct_UClass_UInterface();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler();
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayCueEvent();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent();
// End Cross Module References
class UScriptStruct* FMinimalGameplayCueReplicationProxy::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("MinimalGameplayCueReplicationProxy"), sizeof(FMinimalGameplayCueReplicationProxy), Get_Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FMinimalGameplayCueReplicationProxy>()
{
	return FMinimalGameplayCueReplicationProxy::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FMinimalGameplayCueReplicationProxy(FMinimalGameplayCueReplicationProxy::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("MinimalGameplayCueReplicationProxy"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFMinimalGameplayCueReplicationProxy
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFMinimalGameplayCueReplicationProxy()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("MinimalGameplayCueReplicationProxy")),new UScriptStruct::TCppStructOps<FMinimalGameplayCueReplicationProxy>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFMinimalGameplayCueReplicationProxy;
	struct Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Owner_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Owner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
		{ "ToolTip", "An alternative way to replicating gameplay cues. This does not use fast TArray serialization and does not serialize gameplaycue parameters. The parameters are created on the receiving side with default information.\nThis will be more efficient with server cpu but will take more bandwidth when the array changes.\n\nTo use, put this on your replication proxy actor (such a the pawn). Call SetOwner, PreReplication and RemoveallCues in the appropriate places." },
	};
#endif
	void* Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FMinimalGameplayCueReplicationProxy>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::NewProp_Owner_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::NewProp_Owner = { "Owner", nullptr, (EPropertyFlags)0x0040000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FMinimalGameplayCueReplicationProxy, Owner), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::NewProp_Owner_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::NewProp_Owner_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::NewProp_Owner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"MinimalGameplayCueReplicationProxy",
		sizeof(FMinimalGameplayCueReplicationProxy),
		alignof(FMinimalGameplayCueReplicationProxy),
		Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000205),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("MinimalGameplayCueReplicationProxy"), sizeof(FMinimalGameplayCueReplicationProxy), Get_Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FMinimalGameplayCueReplicationProxy_Hash() { return 4116150762U; }
class UScriptStruct* FGameplayCueTag::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTag_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCueTag, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCueTag"), sizeof(FGameplayCueTag), Get_Z_Construct_UScriptStruct_FGameplayCueTag_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCueTag>()
{
	return FGameplayCueTag::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCueTag(FGameplayCueTag::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCueTag"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTag
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTag()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCueTag")),new UScriptStruct::TCppStructOps<FGameplayCueTag>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTag;
	struct Z_Construct_UScriptStruct_FGameplayCueTag_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueTag_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayCueTag;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTag_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
		{ "ToolTip", "Wrapper struct around a gameplaytag with the GameplayCue category. This also allows for a details customization" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCueTag_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCueTag>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTag_Statics::NewProp_GameplayCueTag_MetaData[] = {
		{ "Categories", "GameplayCue" },
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueTag_Statics::NewProp_GameplayCueTag = { "GameplayCueTag", nullptr, (EPropertyFlags)0x0010000000000005, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTag, GameplayCueTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTag_Statics::NewProp_GameplayCueTag_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTag_Statics::NewProp_GameplayCueTag_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCueTag_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTag_Statics::NewProp_GameplayCueTag,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCueTag_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCueTag",
		sizeof(FGameplayCueTag),
		alignof(FGameplayCueTag),
		Z_Construct_UScriptStruct_FGameplayCueTag_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTag_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTag_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTag_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTag()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTag_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCueTag"), sizeof(FGameplayCueTag), Get_Z_Construct_UScriptStruct_FGameplayCueTag_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCueTag_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTag_Hash() { return 4259064878U; }
class UScriptStruct* FActiveGameplayCueContainer::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FActiveGameplayCueContainer, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ActiveGameplayCueContainer"), sizeof(FActiveGameplayCueContainer), Get_Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FActiveGameplayCueContainer>()
{
	return FActiveGameplayCueContainer::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FActiveGameplayCueContainer(FActiveGameplayCueContainer::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("ActiveGameplayCueContainer"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFActiveGameplayCueContainer
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFActiveGameplayCueContainer()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("ActiveGameplayCueContainer")),new UScriptStruct::TCppStructOps<FActiveGameplayCueContainer>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFActiveGameplayCueContainer;
	struct Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Owner_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Owner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCues_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_GameplayCues;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayCues_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FActiveGameplayCueContainer>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_Owner_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_Owner = { "Owner", nullptr, (EPropertyFlags)0x0040000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FActiveGameplayCueContainer, Owner), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_Owner_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_Owner_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_GameplayCues_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_GameplayCues = { "GameplayCues", nullptr, (EPropertyFlags)0x0010008000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FActiveGameplayCueContainer, GameplayCues), METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_GameplayCues_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_GameplayCues_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_GameplayCues_Inner = { "GameplayCues", nullptr, (EPropertyFlags)0x0000008000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FActiveGameplayCue, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_Owner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_GameplayCues,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::NewProp_GameplayCues_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		Z_Construct_UScriptStruct_FFastArraySerializer,
		&NewStructOps,
		"ActiveGameplayCueContainer",
		sizeof(FActiveGameplayCueContainer),
		alignof(FActiveGameplayCueContainer),
		Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000205),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FActiveGameplayCueContainer()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("ActiveGameplayCueContainer"), sizeof(FActiveGameplayCueContainer), Get_Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FActiveGameplayCueContainer_Hash() { return 873328695U; }
class UScriptStruct* FActiveGameplayCue::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FActiveGameplayCue_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FActiveGameplayCue, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ActiveGameplayCue"), sizeof(FActiveGameplayCue), Get_Z_Construct_UScriptStruct_FActiveGameplayCue_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FActiveGameplayCue>()
{
	return FActiveGameplayCue::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FActiveGameplayCue(FActiveGameplayCue::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("ActiveGameplayCue"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFActiveGameplayCue
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFActiveGameplayCue()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("ActiveGameplayCue")),new UScriptStruct::TCppStructOps<FActiveGameplayCue>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFActiveGameplayCue;
	struct Z_Construct_UScriptStruct_FActiveGameplayCue_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bPredictivelyRemoved_MetaData[];
#endif
		static void NewProp_bPredictivelyRemoved_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bPredictivelyRemoved;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Parameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PredictionKey_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_PredictionKey;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueTag_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayCueTag;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FActiveGameplayCue>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_bPredictivelyRemoved_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
		{ "ToolTip", "Has this been predictively removed on the client?" },
	};
#endif
	void Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_bPredictivelyRemoved_SetBit(void* Obj)
	{
		((FActiveGameplayCue*)Obj)->bPredictivelyRemoved = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_bPredictivelyRemoved = { "bPredictivelyRemoved", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FActiveGameplayCue), &Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_bPredictivelyRemoved_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_bPredictivelyRemoved_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_bPredictivelyRemoved_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_Parameters_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FActiveGameplayCue, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_Parameters_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_PredictionKey_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_PredictionKey = { "PredictionKey", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FActiveGameplayCue, PredictionKey), Z_Construct_UScriptStruct_FPredictionKey, METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_PredictionKey_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_PredictionKey_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_GameplayCueTag_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_GameplayCueTag = { "GameplayCueTag", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FActiveGameplayCue, GameplayCueTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_GameplayCueTag_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_GameplayCueTag_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_bPredictivelyRemoved,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_PredictionKey,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::NewProp_GameplayCueTag,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		Z_Construct_UScriptStruct_FFastArraySerializerItem,
		&NewStructOps,
		"ActiveGameplayCue",
		sizeof(FActiveGameplayCue),
		alignof(FActiveGameplayCue),
		Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000005),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FActiveGameplayCue()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FActiveGameplayCue_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("ActiveGameplayCue"), sizeof(FActiveGameplayCue), Get_Z_Construct_UScriptStruct_FActiveGameplayCue_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FActiveGameplayCue_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FActiveGameplayCue_Hash() { return 1288789632U; }
	void IGameplayCueInterface::BlueprintCustomHandler(EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters)
	{
		check(0 && "Do not directly call Event functions in Interfaces. Call Execute_BlueprintCustomHandler instead.");
	}
	void UGameplayCueInterface::StaticRegisterNativesUGameplayCueInterface()
	{
		UClass* Class = UGameplayCueInterface::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "ForwardGameplayCueToParent", &IGameplayCueInterface::execForwardGameplayCueToParent },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics
	{
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_EventType;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008000000080, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueInterface_eventBlueprintCustomHandler_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::NewProp_EventType = { "EventType", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueInterface_eventBlueprintCustomHandler_Parms, EventType), Z_Construct_UEnum_GameplayAbilities_EGameplayCueEvent, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::NewProp_EventType,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::Function_MetaDataParams[] = {
		{ "BlueprintInternalUseOnly", "true" },
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
		{ "ToolTip", "Internal function to map ufunctions directly to gameplaycue tags" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCueInterface, nullptr, "BlueprintCustomHandler", sizeof(GameplayCueInterface_eventBlueprintCustomHandler_Parms), Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020808, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent_Statics::Function_MetaDataParams[] = {
		{ "Category", "Ability|GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
		{ "ToolTip", "Call from a Cue handler event to continue checking for additional, more generic handlers. Called from the ability system blueprint library" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCueInterface, nullptr, "ForwardGameplayCueToParent", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020408, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UGameplayCueInterface_NoRegister()
	{
		return UGameplayCueInterface::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayCueInterface_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayCueInterface_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UInterface,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UGameplayCueInterface_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayCueInterface_BlueprintCustomHandler, "BlueprintCustomHandler" }, // 976972542
		{ &Z_Construct_UFunction_UGameplayCueInterface_ForwardGameplayCueToParent, "ForwardGameplayCueToParent" }, // 620460201
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueInterface_Statics::Class_MetaDataParams[] = {
		{ "CannotImplementInterfaceInBlueprint", "" },
		{ "ModuleRelativePath", "Public/GameplayCueInterface.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayCueInterface_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<IGameplayCueInterface>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCueInterface_Statics::ClassParams = {
		&UGameplayCueInterface::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x000840A1u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayCueInterface_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueInterface_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayCueInterface()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayCueInterface_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayCueInterface, 1612358386);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayCueInterface>()
	{
		return UGameplayCueInterface::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayCueInterface(Z_Construct_UClass_UGameplayCueInterface, &UGameplayCueInterface::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayCueInterface"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCueInterface);
	static FName NAME_UGameplayCueInterface_BlueprintCustomHandler = FName(TEXT("BlueprintCustomHandler"));
	void IGameplayCueInterface::Execute_BlueprintCustomHandler(UObject* O, EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters)
	{
		check(O != NULL);
		check(O->GetClass()->ImplementsInterface(UGameplayCueInterface::StaticClass()));
		GameplayCueInterface_eventBlueprintCustomHandler_Parms Parms;
		UFunction* const Func = O->FindFunction(NAME_UGameplayCueInterface_BlueprintCustomHandler);
		if (Func)
		{
			Parms.EventType=EventType;
			Parms.Parameters=Parameters;
			O->ProcessEvent(Func, &Parms);
		}
	}
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
