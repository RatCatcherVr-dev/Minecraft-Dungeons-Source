// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayTagResponseTable.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayTagResponseTable() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTagReponsePair();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayTagReponseTable_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayTagReponseTable();
	ENGINE_API UClass* Z_Construct_UClass_UDataAsset();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent_NoRegister();
// End Cross Module References
class UScriptStruct* FGameplayTagResponseTableEntry::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayTagResponseTableEntry"), sizeof(FGameplayTagResponseTableEntry), Get_Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayTagResponseTableEntry>()
{
	return FGameplayTagResponseTableEntry::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayTagResponseTableEntry(FGameplayTagResponseTableEntry::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayTagResponseTableEntry"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTagResponseTableEntry
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTagResponseTableEntry()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayTagResponseTableEntry")),new UScriptStruct::TCppStructOps<FGameplayTagResponseTableEntry>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTagResponseTableEntry;
	struct Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Negative_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Negative;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Positive_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Positive;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayTagResponseTableEntry>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Negative_MetaData[] = {
		{ "Category", "Response" },
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
		{ "ToolTip", "Tags that count as \"negative\" toward to final response count. If the overall count is negative, this ResponseGameplayEffect is applied." },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Negative = { "Negative", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTagResponseTableEntry, Negative), Z_Construct_UScriptStruct_FGameplayTagReponsePair, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Negative_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Negative_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Positive_MetaData[] = {
		{ "Category", "Response" },
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
		{ "ToolTip", "Tags that count as \"positive\" toward to final response count. If the overall count is positive, this ResponseGameplayEffect is applied." },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Positive = { "Positive", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTagResponseTableEntry, Positive), Z_Construct_UScriptStruct_FGameplayTagReponsePair, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Positive_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Positive_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Negative,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::NewProp_Positive,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayTagResponseTableEntry",
		sizeof(FGameplayTagResponseTableEntry),
		alignof(FGameplayTagResponseTableEntry),
		Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayTagResponseTableEntry"), sizeof(FGameplayTagResponseTableEntry), Get_Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry_Hash() { return 1199130823U; }
class UScriptStruct* FGameplayTagReponsePair::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayTagReponsePair_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayTagReponsePair, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayTagReponsePair"), sizeof(FGameplayTagReponsePair), Get_Z_Construct_UScriptStruct_FGameplayTagReponsePair_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayTagReponsePair>()
{
	return FGameplayTagReponsePair::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayTagReponsePair(FGameplayTagReponsePair::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayTagReponsePair"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTagReponsePair
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTagReponsePair()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayTagReponsePair")),new UScriptStruct::TCppStructOps<FGameplayTagReponsePair>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTagReponsePair;
	struct Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SoftCountCap_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_SoftCountCap;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ResponseGameplayEffects_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_ResponseGameplayEffects;
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_ResponseGameplayEffects_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ResponseGameplayEffect_MetaData[];
#endif
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_ResponseGameplayEffect;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Tag_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Tag;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayTagReponsePair>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_SoftCountCap_MetaData[] = {
		{ "Category", "Response" },
		{ "ClampMin", "0" },
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
		{ "ToolTip", "The max \"count\" this response can achieve. This will not prevent counts from being applied, but will be used when calculating the net count of a tag. 0=no cap." },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_SoftCountCap = { "SoftCountCap", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTagReponsePair, SoftCountCap), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_SoftCountCap_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_SoftCountCap_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffects_MetaData[] = {
		{ "Category", "Response" },
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
		{ "ToolTip", "The GameplayEffects to apply in reponse to the tag" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffects = { "ResponseGameplayEffects", nullptr, (EPropertyFlags)0x0014000000000001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTagReponsePair, ResponseGameplayEffects), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffects_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffects_MetaData)) };
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffects_Inner = { "ResponseGameplayEffects", nullptr, (EPropertyFlags)0x0004000000000000, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UGameplayEffect_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffect_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
		{ "ToolTip", "Deprecated. Replaced with ResponseGameplayEffects" },
	};
#endif
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffect = { "ResponseGameplayEffect", nullptr, (EPropertyFlags)0x0014000000000000, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTagReponsePair, ResponseGameplayEffect), Z_Construct_UClass_UGameplayEffect_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffect_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffect_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_Tag_MetaData[] = {
		{ "Category", "Response" },
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
		{ "ToolTip", "Tag that triggers this response" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_Tag = { "Tag", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTagReponsePair, Tag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_Tag_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_Tag_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_SoftCountCap,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffects,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffects_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_ResponseGameplayEffect,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::NewProp_Tag,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayTagReponsePair",
		sizeof(FGameplayTagReponsePair),
		alignof(FGameplayTagReponsePair),
		Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayTagReponsePair()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayTagReponsePair_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayTagReponsePair"), sizeof(FGameplayTagReponsePair), Get_Z_Construct_UScriptStruct_FGameplayTagReponsePair_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayTagReponsePair_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayTagReponsePair_Hash() { return 3642691333U; }
	void UGameplayTagReponseTable::StaticRegisterNativesUGameplayTagReponseTable()
	{
		UClass* Class = UGameplayTagReponseTable::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "TagResponseEvent", &UGameplayTagReponseTable::execTagResponseEvent },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics
	{
		struct GameplayTagReponseTable_eventTagResponseEvent_Parms
		{
			FGameplayTag Tag;
			int32 NewCount;
			UAbilitySystemComponent* ASC;
			int32 idx;
		};
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_idx;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ASC_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ASC;
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_NewCount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Tag_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Tag;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_idx = { "idx", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayTagReponseTable_eventTagResponseEvent_Parms, idx), METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_ASC_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_ASC = { "ASC", nullptr, (EPropertyFlags)0x0010000000080080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayTagReponseTable_eventTagResponseEvent_Parms, ASC), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_ASC_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_ASC_MetaData)) };
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_NewCount = { "NewCount", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayTagReponseTable_eventTagResponseEvent_Parms, NewCount), METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_Tag_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_Tag = { "Tag", nullptr, (EPropertyFlags)0x0010000000000082, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayTagReponseTable_eventTagResponseEvent_Parms, Tag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_Tag_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_Tag_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_idx,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_ASC,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_NewCount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::NewProp_Tag,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayTagReponseTable, nullptr, "TagResponseEvent", sizeof(GameplayTagReponseTable_eventTagResponseEvent_Parms), Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UGameplayTagReponseTable_NoRegister()
	{
		return UGameplayTagReponseTable::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayTagReponseTable_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Entries_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Entries;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Entries_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayTagReponseTable_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UDataAsset,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UGameplayTagReponseTable_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayTagReponseTable_TagResponseEvent, "TagResponseEvent" }, // 952542889
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayTagReponseTable_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayTagResponseTable.h" },
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
		{ "ToolTip", "A data driven table for applying gameplay effects based on tag count. This allows designers to map a\n\"tag count\" -> \"response Gameplay Effect\" relationship.\n\nFor example, \"for every count of \"Status.Haste\" I get 1 level of GE_Response_Haste. This class facilitates\nbuilding this table and automatically registering and responding to tag events on the ability system component." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayTagReponseTable_Statics::NewProp_Entries_MetaData[] = {
		{ "Category", "Response" },
		{ "ModuleRelativePath", "Public/GameplayTagResponseTable.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayTagReponseTable_Statics::NewProp_Entries = { "Entries", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayTagReponseTable, Entries), METADATA_PARAMS(Z_Construct_UClass_UGameplayTagReponseTable_Statics::NewProp_Entries_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayTagReponseTable_Statics::NewProp_Entries_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayTagReponseTable_Statics::NewProp_Entries_Inner = { "Entries", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayTagResponseTableEntry, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayTagReponseTable_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayTagReponseTable_Statics::NewProp_Entries,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayTagReponseTable_Statics::NewProp_Entries_Inner,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayTagReponseTable_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayTagReponseTable>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayTagReponseTable_Statics::ClassParams = {
		&UGameplayTagReponseTable::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UGameplayTagReponseTable_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UGameplayTagReponseTable_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayTagReponseTable_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayTagReponseTable_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayTagReponseTable()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayTagReponseTable_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayTagReponseTable, 2018204419);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayTagReponseTable>()
	{
		return UGameplayTagReponseTable::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayTagReponseTable(Z_Construct_UClass_UGameplayTagReponseTable, &UGameplayTagReponseTable::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayTagReponseTable"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayTagReponseTable);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
