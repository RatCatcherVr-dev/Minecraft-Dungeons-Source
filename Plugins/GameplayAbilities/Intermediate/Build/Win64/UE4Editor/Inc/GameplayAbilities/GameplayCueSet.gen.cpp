// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCueSet.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCueSet() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueNotifyData();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject_NoRegister();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FSoftObjectPath();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueSet_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueSet();
	ENGINE_API UClass* Z_Construct_UClass_UDataAsset();
// End Cross Module References
class UScriptStruct* FGameplayCueNotifyData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCueNotifyData_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCueNotifyData, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCueNotifyData"), sizeof(FGameplayCueNotifyData), Get_Z_Construct_UScriptStruct_FGameplayCueNotifyData_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCueNotifyData>()
{
	return FGameplayCueNotifyData::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCueNotifyData(FGameplayCueNotifyData::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCueNotifyData"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueNotifyData
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueNotifyData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCueNotifyData")),new UScriptStruct::TCppStructOps<FGameplayCueNotifyData>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCueNotifyData;
	struct Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LoadedGameplayCueClass_MetaData[];
#endif
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_LoadedGameplayCueClass;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueNotifyObj_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayCueNotifyObj;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueTag_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayCueTag;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueSet.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCueNotifyData>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_LoadedGameplayCueClass_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueSet.h" },
	};
#endif
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_LoadedGameplayCueClass = { "LoadedGameplayCueClass", nullptr, (EPropertyFlags)0x0010000000002000, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueNotifyData, LoadedGameplayCueClass), Z_Construct_UClass_UObject_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_LoadedGameplayCueClass_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_LoadedGameplayCueClass_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueNotifyObj_MetaData[] = {
		{ "AllowedClasses", "GameplayCueNotify" },
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueSet.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueNotifyObj = { "GameplayCueNotifyObj", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueNotifyData, GameplayCueNotifyObj), Z_Construct_UScriptStruct_FSoftObjectPath, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueNotifyObj_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueNotifyObj_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueTag_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueSet.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueTag = { "GameplayCueTag", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCueNotifyData, GameplayCueTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueTag_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueTag_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_LoadedGameplayCueClass,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueNotifyObj,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::NewProp_GameplayCueTag,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCueNotifyData",
		sizeof(FGameplayCueNotifyData),
		alignof(FGameplayCueNotifyData),
		Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueNotifyData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCueNotifyData_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCueNotifyData"), sizeof(FGameplayCueNotifyData), Get_Z_Construct_UScriptStruct_FGameplayCueNotifyData_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCueNotifyData_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCueNotifyData_Hash() { return 257269255U; }
	void UGameplayCueSet::StaticRegisterNativesUGameplayCueSet()
	{
	}
	UClass* Z_Construct_UClass_UGameplayCueSet_NoRegister()
	{
		return UGameplayCueSet::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayCueSet_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueData_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_GameplayCueData;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayCueData_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayCueSet_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UDataAsset,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueSet_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayCueSet.h" },
		{ "ModuleRelativePath", "Public/GameplayCueSet.h" },
		{ "ToolTip", "A set of gameplay cue actors to handle gameplay cue events" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueSet_Statics::NewProp_GameplayCueData_MetaData[] = {
		{ "Category", "CueSet" },
		{ "ModuleRelativePath", "Public/GameplayCueSet.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayCueSet_Statics::NewProp_GameplayCueData = { "GameplayCueData", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueSet, GameplayCueData), METADATA_PARAMS(Z_Construct_UClass_UGameplayCueSet_Statics::NewProp_GameplayCueData_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueSet_Statics::NewProp_GameplayCueData_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayCueSet_Statics::NewProp_GameplayCueData_Inner = { "GameplayCueData", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayCueNotifyData, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayCueSet_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueSet_Statics::NewProp_GameplayCueData,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueSet_Statics::NewProp_GameplayCueData_Inner,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayCueSet_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCueSet>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCueSet_Statics::ClassParams = {
		&UGameplayCueSet::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayCueSet_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayCueSet_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayCueSet_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueSet_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayCueSet()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayCueSet_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayCueSet, 3085428372);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayCueSet>()
	{
		return UGameplayCueSet::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayCueSet(Z_Construct_UClass_UGameplayCueSet, &UGameplayCueSet::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayCueSet"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCueSet);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
