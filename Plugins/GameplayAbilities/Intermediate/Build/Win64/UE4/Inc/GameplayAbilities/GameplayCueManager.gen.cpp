// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCueManager.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCueManager() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueObjectLibrary();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueSet_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UObjectLibrary_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueManager_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueManager();
	ENGINE_API UClass* Z_Construct_UClass_UDataAsset();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FPreallocationInfo();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCuePendingExecute();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayCueNotify_Actor_NoRegister();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject_NoRegister();
// End Cross Module References
class UScriptStruct* FGameplayCueObjectLibrary::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCueObjectLibrary"), sizeof(FGameplayCueObjectLibrary), Get_Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCueObjectLibrary>()
{
	return FGameplayCueObjectLibrary::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCueObjectLibrary(FGameplayCueObjectLibrary::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCueObjectLibrary"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueObjectLibrary
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueObjectLibrary()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCueObjectLibrary")),new UScriptStruct::TCppStructOps<FGameplayCueObjectLibrary>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueObjectLibrary;
	struct Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bHasBeenInitialized_MetaData[];
#endif
		static void NewProp_bHasBeenInitialized_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bHasBeenInitialized;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bShouldSyncLoad_MetaData[];
#endif
		static void NewProp_bShouldSyncLoad_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bShouldSyncLoad;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bShouldAsyncLoad_MetaData[];
#endif
		static void NewProp_bShouldAsyncLoad_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bShouldAsyncLoad;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bShouldSyncScan_MetaData[];
#endif
		static void NewProp_bShouldSyncScan_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bShouldSyncScan;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CueSet_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_CueSet;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticObjectLibrary_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_StaticObjectLibrary;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ActorObjectLibrary_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ActorObjectLibrary;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Paths_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Paths;
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_Paths_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "An ObjectLibrary for the GameplayCue Notifies. Wraps 2 underlying UObjectLibraries plus options/delegates for how they are loaded" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCueObjectLibrary>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bHasBeenInitialized_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "True if this has been initialized with correct data" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bHasBeenInitialized_SetBit(void* Obj)
	{
		((FGameplayCueObjectLibrary*)Obj)->bHasBeenInitialized = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bHasBeenInitialized = { "bHasBeenInitialized", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FGameplayCueObjectLibrary), &Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bHasBeenInitialized_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bHasBeenInitialized_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bHasBeenInitialized_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncLoad_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Should we sync load everything that we find (that passes ShouldLoad delegate check)" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncLoad_SetBit(void* Obj)
	{
		((FGameplayCueObjectLibrary*)Obj)->bShouldSyncLoad = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncLoad = { "bShouldSyncLoad", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FGameplayCueObjectLibrary), &Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncLoad_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncLoad_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncLoad_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldAsyncLoad_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Should we start async loading everything that we find (that passes ShouldLoad delegate check)" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldAsyncLoad_SetBit(void* Obj)
	{
		((FGameplayCueObjectLibrary*)Obj)->bShouldAsyncLoad = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldAsyncLoad = { "bShouldAsyncLoad", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FGameplayCueObjectLibrary), &Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldAsyncLoad_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldAsyncLoad_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldAsyncLoad_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncScan_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Should we force a sync scan on the asset registry in order to discover asset data, or just use what is there" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncScan_SetBit(void* Obj)
	{
		((FGameplayCueObjectLibrary*)Obj)->bShouldSyncScan = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncScan = { "bShouldSyncScan", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FGameplayCueObjectLibrary), &Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncScan_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncScan_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncScan_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_CueSet_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Set to put the loaded asset data into. If null we will use the global set (RuntimeGameplayCueObjectLibrary.CueSet)" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_CueSet = { "CueSet", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueObjectLibrary, CueSet), Z_Construct_UClass_UGameplayCueSet_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_CueSet_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_CueSet_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_StaticObjectLibrary_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Object library for object based notifies" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_StaticObjectLibrary = { "StaticObjectLibrary", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueObjectLibrary, StaticObjectLibrary), Z_Construct_UClass_UObjectLibrary_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_StaticObjectLibrary_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_StaticObjectLibrary_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_ActorObjectLibrary_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Object library for actor based notifies" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_ActorObjectLibrary = { "ActorObjectLibrary", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueObjectLibrary, ActorObjectLibrary), Z_Construct_UClass_UObjectLibrary_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_ActorObjectLibrary_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_ActorObjectLibrary_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_Paths_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Paths to search for" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_Paths = { "Paths", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueObjectLibrary, Paths), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_Paths_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_Paths_MetaData)) };
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_Paths_Inner = { "Paths", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bHasBeenInitialized,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncLoad,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldAsyncLoad,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_bShouldSyncScan,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_CueSet,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_StaticObjectLibrary,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_ActorObjectLibrary,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_Paths,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::NewProp_Paths_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCueObjectLibrary",
		sizeof(FGameplayCueObjectLibrary),
		alignof(FGameplayCueObjectLibrary),
		Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueObjectLibrary()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCueObjectLibrary"), sizeof(FGameplayCueObjectLibrary), Get_Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCueObjectLibrary_Hash() { return 2974946227U; }
	void UGameplayCueManager::StaticRegisterNativesUGameplayCueManager()
	{
	}
	UClass* Z_Construct_UClass_UGameplayCueManager_NoRegister()
	{
		return UGameplayCueManager::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayCueManager_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PreallocationInfoList_Internal_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_PreallocationInfoList_Internal;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_PreallocationInfoList_Internal_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueSendContextCount_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_GameplayCueSendContextCount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PendingExecuteCues_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_PendingExecuteCues;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_PendingExecuteCues_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueClassesForPreallocation_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_GameplayCueClassesForPreallocation;
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_GameplayCueClassesForPreallocation_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LoadedGameplayCueNotifyClasses_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_LoadedGameplayCueNotifyClasses;
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_LoadedGameplayCueNotifyClasses_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_EditorGameplayCueObjectLibrary_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_EditorGameplayCueObjectLibrary;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_RuntimeGameplayCueObjectLibrary_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_RuntimeGameplayCueObjectLibrary;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayCueManager_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UDataAsset,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayCueManager.h" },
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Singleton manager object that handles dispatching gameplay cues and spawning GameplayCueNotify actors as needed" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PreallocationInfoList_Internal_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PreallocationInfoList_Internal = { "PreallocationInfoList_Internal", nullptr, (EPropertyFlags)0x0020080000002000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueManager, PreallocationInfoList_Internal), METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PreallocationInfoList_Internal_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PreallocationInfoList_Internal_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PreallocationInfoList_Internal_Inner = { "PreallocationInfoList_Internal", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FPreallocationInfo, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueSendContextCount_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Number of active gameplay cue send contexts, when it goes to 0 cues are flushed" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueSendContextCount = { "GameplayCueSendContextCount", nullptr, (EPropertyFlags)0x0020080000002000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueManager, GameplayCueSendContextCount), METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueSendContextCount_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueSendContextCount_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PendingExecuteCues_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "List of gameplay cue executes that haven't been processed yet" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PendingExecuteCues = { "PendingExecuteCues", nullptr, (EPropertyFlags)0x0020088000002000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueManager, PendingExecuteCues), METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PendingExecuteCues_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PendingExecuteCues_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PendingExecuteCues_Inner = { "PendingExecuteCues", nullptr, (EPropertyFlags)0x0000008000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayCuePendingExecute, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueClassesForPreallocation_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Classes that we need to preallocate instances for" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueClassesForPreallocation = { "GameplayCueClassesForPreallocation", nullptr, (EPropertyFlags)0x0024080000002000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueManager, GameplayCueClassesForPreallocation), METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueClassesForPreallocation_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueClassesForPreallocation_MetaData)) };
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueClassesForPreallocation_Inner = { "GameplayCueClassesForPreallocation", nullptr, (EPropertyFlags)0x0004000000000000, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_AGameplayCueNotify_Actor_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_LoadedGameplayCueNotifyClasses_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
		{ "ToolTip", "Hardref to the gameplaycue notify classes we have async loaded" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_LoadedGameplayCueNotifyClasses = { "LoadedGameplayCueNotifyClasses", nullptr, (EPropertyFlags)0x0020080000002000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueManager, LoadedGameplayCueNotifyClasses), METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_LoadedGameplayCueNotifyClasses_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_LoadedGameplayCueNotifyClasses_MetaData)) };
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_LoadedGameplayCueNotifyClasses_Inner = { "LoadedGameplayCueNotifyClasses", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UObject_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_EditorGameplayCueObjectLibrary_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_EditorGameplayCueObjectLibrary = { "EditorGameplayCueObjectLibrary", nullptr, (EPropertyFlags)0x0020080000002000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueManager, EditorGameplayCueObjectLibrary), Z_Construct_UScriptStruct_FGameplayCueObjectLibrary, METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_EditorGameplayCueObjectLibrary_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_EditorGameplayCueObjectLibrary_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_RuntimeGameplayCueObjectLibrary_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueManager.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_RuntimeGameplayCueObjectLibrary = { "RuntimeGameplayCueObjectLibrary", nullptr, (EPropertyFlags)0x0020080000002000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueManager, RuntimeGameplayCueObjectLibrary), Z_Construct_UScriptStruct_FGameplayCueObjectLibrary, METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_RuntimeGameplayCueObjectLibrary_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_RuntimeGameplayCueObjectLibrary_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayCueManager_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PreallocationInfoList_Internal,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PreallocationInfoList_Internal_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueSendContextCount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PendingExecuteCues,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_PendingExecuteCues_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueClassesForPreallocation,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_GameplayCueClassesForPreallocation_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_LoadedGameplayCueNotifyClasses,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_LoadedGameplayCueNotifyClasses_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_EditorGameplayCueObjectLibrary,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueManager_Statics::NewProp_RuntimeGameplayCueObjectLibrary,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayCueManager_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCueManager>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCueManager_Statics::ClassParams = {
		&UGameplayCueManager::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayCueManager_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::PropPointers),
		0,
		0x009000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayCueManager_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueManager_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayCueManager()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayCueManager_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayCueManager, 2746934151);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayCueManager>()
	{
		return UGameplayCueManager::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayCueManager(Z_Construct_UClass_UGameplayCueManager, &UGameplayCueManager::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayCueManager"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCueManager);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
