// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCueTranslator.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCueTranslator() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslationManager();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYTAGS_API UClass* Z_Construct_UClass_UGameplayTagsManager_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslatorNode();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslationLink();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueTranslator_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueTranslator();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueTranslator_Test_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueTranslator_Test();
// End Cross Module References
class UScriptStruct* FGameplayCueTranslationManager::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCueTranslationManager, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCueTranslationManager"), sizeof(FGameplayCueTranslationManager), Get_Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCueTranslationManager>()
{
	return FGameplayCueTranslationManager::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCueTranslationManager(FGameplayCueTranslationManager::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCueTranslationManager"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslationManager
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslationManager()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCueTranslationManager")),new UScriptStruct::TCppStructOps<FGameplayCueTranslationManager>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslationManager;
	struct Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TagManager_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_TagManager;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TranslationNameToIndexMap_MetaData[];
#endif
		static const UE4CodeGen_Private::FMapPropertyParams NewProp_TranslationNameToIndexMap;
		static const UE4CodeGen_Private::FNamePropertyParams NewProp_TranslationNameToIndexMap_Key_KeyProp;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_TranslationNameToIndexMap_ValueProp;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TranslationLUT_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_TranslationLUT;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_TranslationLUT_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "This is the struct that does the actual translation. It lives on the GameplayCueManager and encapsulates all translation logic." },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCueTranslationManager>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TagManager_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "Cached reference to tag manager" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TagManager = { "TagManager", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslationManager, TagManager), Z_Construct_UClass_UGameplayTagsManager_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TagManager_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TagManager_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "Acceleration map from gameplay tag name to index into TranslationLUT" },
	};
#endif
	const UE4CodeGen_Private::FMapPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap = { "TranslationNameToIndexMap", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslationManager, TranslationNameToIndexMap), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap_MetaData)) };
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap_Key_KeyProp = { "TranslationNameToIndexMap_Key", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap_ValueProp = { "TranslationNameToIndexMap", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 1, Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationLUT_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "The Look Up Table" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationLUT = { "TranslationLUT", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslationManager, TranslationLUT), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationLUT_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationLUT_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationLUT_Inner = { "TranslationLUT", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayCueTranslatorNode, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TagManager,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap_Key_KeyProp,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationNameToIndexMap_ValueProp,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationLUT,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::NewProp_TranslationLUT_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCueTranslationManager",
		sizeof(FGameplayCueTranslationManager),
		alignof(FGameplayCueTranslationManager),
		Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslationManager()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCueTranslationManager"), sizeof(FGameplayCueTranslationManager), Get_Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslationManager_Hash() { return 1133684686U; }
class UScriptStruct* FGameplayCueTranslatorNode::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCueTranslatorNode"), sizeof(FGameplayCueTranslatorNode), Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCueTranslatorNode>()
{
	return FGameplayCueTranslatorNode::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCueTranslatorNode(FGameplayCueTranslatorNode::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCueTranslatorNode"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslatorNode
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslatorNode()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCueTranslatorNode")),new UScriptStruct::TCppStructOps<FGameplayCueTranslatorNode>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslatorNode;
	struct Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CachedGameplayTagName_MetaData[];
#endif
		static const UE4CodeGen_Private::FNamePropertyParams NewProp_CachedGameplayTagName;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CachedGameplayTag_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CachedGameplayTag;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CachedIndex_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CachedIndex;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Links_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Links;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Links_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "A node in our translation table/graph. The node represents an actual gameplaytag or a possible gameplay tag, with links to what it can be translated into." },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCueTranslatorNode>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTagName_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "FName of the tag. This will always be valid, whether the tag is in the GameplayTag dictionary or not." },
	};
#endif
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTagName = { "CachedGameplayTagName", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslatorNode, CachedGameplayTagName), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTagName_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTagName_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTag_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "The FGameplayTag if this tag actually exists. This will always exist at runtime. In editor builds, it may not (GameplayCueEditor)" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTag = { "CachedGameplayTag", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslatorNode, CachedGameplayTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTag_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTag_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedIndex_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "Our index into FGameplayCueTranslationManager::TranslationLUT" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedIndex = { "CachedIndex", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslatorNode, CachedIndex), Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedIndex_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedIndex_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_Links_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "Ways we can be translated into another FGameplayCueTranslatorNode" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_Links = { "Links", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslatorNode, Links), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_Links_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_Links_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_Links_Inner = { "Links", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayCueTranslationLink, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTagName,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedGameplayTag,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_CachedIndex,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_Links,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::NewProp_Links_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCueTranslatorNode",
		sizeof(FGameplayCueTranslatorNode),
		alignof(FGameplayCueTranslatorNode),
		Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslatorNode()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCueTranslatorNode"), sizeof(FGameplayCueTranslatorNode), Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNode_Hash() { return 2805613851U; }
class UScriptStruct* FGameplayCueTranslationLink::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCueTranslationLink, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCueTranslationLink"), sizeof(FGameplayCueTranslationLink), Get_Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCueTranslationLink>()
{
	return FGameplayCueTranslationLink::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCueTranslationLink(FGameplayCueTranslationLink::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCueTranslationLink"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslationLink
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslationLink()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCueTranslationLink")),new UScriptStruct::TCppStructOps<FGameplayCueTranslationLink>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslationLink;
	struct Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_RulesCDO_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_RulesCDO;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "Represents a translation from one FGameplayCueTranslatorNode to many others. You will have one of these links per UGameplayCueTranslator that can translate a node." },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCueTranslationLink>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::NewProp_RulesCDO_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "NativeConst", "" },
		{ "ToolTip", "The rule that provides this translation" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::NewProp_RulesCDO = { "RulesCDO", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslationLink, RulesCDO), Z_Construct_UClass_UGameplayCueTranslator_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::NewProp_RulesCDO_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::NewProp_RulesCDO_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::NewProp_RulesCDO,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCueTranslationLink",
		sizeof(FGameplayCueTranslationLink),
		alignof(FGameplayCueTranslationLink),
		Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslationLink()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCueTranslationLink"), sizeof(FGameplayCueTranslationLink), Get_Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslationLink_Hash() { return 1882403771U; }
class UScriptStruct* FGameplayCueTranslatorNodeIndex::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCueTranslatorNodeIndex"), sizeof(FGameplayCueTranslatorNodeIndex), Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCueTranslatorNodeIndex>()
{
	return FGameplayCueTranslatorNodeIndex::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCueTranslatorNodeIndex(FGameplayCueTranslatorNodeIndex::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCueTranslatorNodeIndex"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslatorNodeIndex
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslatorNodeIndex()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCueTranslatorNodeIndex")),new UScriptStruct::TCppStructOps<FGameplayCueTranslatorNodeIndex>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueTranslatorNodeIndex;
	struct Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Index_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_Index;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "Simple index/handle for referencing items in FGameplayCueTranslationManager::TranslationLUT" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCueTranslatorNodeIndex>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::NewProp_Index_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::NewProp_Index = { "Index", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueTranslatorNodeIndex, Index), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::NewProp_Index_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::NewProp_Index_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::NewProp_Index,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCueTranslatorNodeIndex",
		sizeof(FGameplayCueTranslatorNodeIndex),
		alignof(FGameplayCueTranslatorNodeIndex),
		Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCueTranslatorNodeIndex"), sizeof(FGameplayCueTranslatorNodeIndex), Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCueTranslatorNodeIndex_Hash() { return 2693223992U; }
	void UGameplayCueTranslator::StaticRegisterNativesUGameplayCueTranslator()
	{
	}
	UClass* Z_Construct_UClass_UGameplayCueTranslator_NoRegister()
	{
		return UGameplayCueTranslator::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayCueTranslator_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayCueTranslator_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueTranslator_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayCueTranslator.h" },
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "This is the base class for GameplayCue Translators. This is what games must extend from in order to add their own rules.\nThese are not instantiated, and are basically just a holder for virtual functions that are called on the CDO.\n\nThere are two main things this class provides:\n        1. A set of translation rules. E.g., \"I translate GameplayCue.A.B.C into GameplayCue.X.B.C\", or rather \"I translate A into X\". (GetTranslationNameSpawns)\n        2. A runtime function to actually do the translation, based on the actors and parameters involved in the gameplay cue event. (GameplayCueToTranslationIndex)" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayCueTranslator_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCueTranslator>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCueTranslator_Statics::ClassParams = {
		&UGameplayCueTranslator::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x001000A1u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayCueTranslator_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueTranslator_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayCueTranslator()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayCueTranslator_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayCueTranslator, 2359148552);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayCueTranslator>()
	{
		return UGameplayCueTranslator::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayCueTranslator(Z_Construct_UClass_UGameplayCueTranslator, &UGameplayCueTranslator::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayCueTranslator"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCueTranslator);
	void UGameplayCueTranslator_Test::StaticRegisterNativesUGameplayCueTranslator_Test()
	{
	}
	UClass* Z_Construct_UClass_UGameplayCueTranslator_Test_NoRegister()
	{
		return UGameplayCueTranslator_Test::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayCueTranslator_Test_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayCueTranslator_Test_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayCueTranslator,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueTranslator_Test_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayCueTranslator.h" },
		{ "ModuleRelativePath", "Public/GameplayCueTranslator.h" },
		{ "ToolTip", "This is an example translator class." },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayCueTranslator_Test_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCueTranslator_Test>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCueTranslator_Test_Statics::ClassParams = {
		&UGameplayCueTranslator_Test::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x000000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayCueTranslator_Test_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueTranslator_Test_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayCueTranslator_Test()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayCueTranslator_Test_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayCueTranslator_Test, 4122832961);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayCueTranslator_Test>()
	{
		return UGameplayCueTranslator_Test::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayCueTranslator_Test(Z_Construct_UClass_UGameplayCueTranslator_Test, &UGameplayCueTranslator_Test::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayCueTranslator_Test"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCueTranslator_Test);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
