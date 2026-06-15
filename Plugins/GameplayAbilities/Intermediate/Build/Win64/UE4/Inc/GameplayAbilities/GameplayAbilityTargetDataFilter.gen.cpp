// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityTargetDataFilter.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilityTargetDataFilter() {}
// Cross Module References
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_ETargetDataFilterSelf();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTargetDataFilter();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
// End Cross Module References
	static UEnum* ETargetDataFilterSelf_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_ETargetDataFilterSelf, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ETargetDataFilterSelf"));
		}
		return Singleton;
	}
	template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<ETargetDataFilterSelf::Type>()
	{
		return ETargetDataFilterSelf_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_ETargetDataFilterSelf(ETargetDataFilterSelf_StaticEnum, TEXT("/Script/GameplayAbilities"), TEXT("ETargetDataFilterSelf"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_GameplayAbilities_ETargetDataFilterSelf_Hash() { return 3014527045U; }
	UEnum* Z_Construct_UEnum_GameplayAbilities_ETargetDataFilterSelf()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("ETargetDataFilterSelf"), 0, Get_Z_Construct_UEnum_GameplayAbilities_ETargetDataFilterSelf_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "ETargetDataFilterSelf::TDFS_Any", (int64)ETargetDataFilterSelf::TDFS_Any },
				{ "ETargetDataFilterSelf::TDFS_NoSelf", (int64)ETargetDataFilterSelf::TDFS_NoSelf },
				{ "ETargetDataFilterSelf::TDFS_NoOthers", (int64)ETargetDataFilterSelf::TDFS_NoOthers },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "BlueprintType", "true" },
				{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetDataFilter.h" },
				{ "TDFS_Any.DisplayName", "Allow self or others" },
				{ "TDFS_NoOthers.DisplayName", "Filter others out" },
				{ "TDFS_NoSelf.DisplayName", "Filter self out" },
				{ "ToolTip", "Set rather it is possible to target self" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
				nullptr,
				"ETargetDataFilterSelf",
				"ETargetDataFilterSelf::Type",
				Enumerators,
				ARRAY_COUNT(Enumerators),
				RF_Public|RF_Transient|RF_MarkAsNative,
				UE4CodeGen_Private::EDynamicType::NotDynamic,
				(uint8)UEnum::ECppForm::Namespaced,
				METADATA_PARAMS(Enum_MetaDataParams, ARRAY_COUNT(Enum_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUEnum(ReturnEnum, EnumParams);
		}
		return ReturnEnum;
	}
class UScriptStruct* FGameplayTargetDataFilterHandle::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayTargetDataFilterHandle"), sizeof(FGameplayTargetDataFilterHandle), Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayTargetDataFilterHandle>()
{
	return FGameplayTargetDataFilterHandle::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayTargetDataFilterHandle(FGameplayTargetDataFilterHandle::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayTargetDataFilterHandle"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTargetDataFilterHandle
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTargetDataFilterHandle()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayTargetDataFilterHandle")),new UScriptStruct::TCppStructOps<FGameplayTargetDataFilterHandle>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTargetDataFilterHandle;
	struct Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetDataFilter.h" },
		{ "ToolTip", "Polymorphic handle to filter structure that handles checking if actors should be targeted" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayTargetDataFilterHandle>();
	}
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayTargetDataFilterHandle",
		sizeof(FGameplayTargetDataFilterHandle),
		alignof(FGameplayTargetDataFilterHandle),
		nullptr,
		0,
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayTargetDataFilterHandle"), sizeof(FGameplayTargetDataFilterHandle), Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilterHandle_Hash() { return 2014669712U; }
class UScriptStruct* FGameplayTargetDataFilter::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayTargetDataFilter, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayTargetDataFilter"), sizeof(FGameplayTargetDataFilter), Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayTargetDataFilter>()
{
	return FGameplayTargetDataFilter::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayTargetDataFilter(FGameplayTargetDataFilter::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayTargetDataFilter"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTargetDataFilter
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTargetDataFilter()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayTargetDataFilter")),new UScriptStruct::TCppStructOps<FGameplayTargetDataFilter>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayTargetDataFilter;
	struct Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bReverseFilter_MetaData[];
#endif
		static void NewProp_bReverseFilter_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bReverseFilter;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_RequiredActorClass_MetaData[];
#endif
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_RequiredActorClass;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SelfFilter_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_SelfFilter;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SelfActor_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SelfActor;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetDataFilter.h" },
		{ "ToolTip", "Simple actor target filter, games can subclass this" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayTargetDataFilter>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_bReverseFilter_MetaData[] = {
		{ "Category", "Filter" },
		{ "ExposeOnSpawn", "TRUE" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetDataFilter.h" },
		{ "ToolTip", "Reverses the meaning of the filter, so it will exclude all actors that pass." },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_bReverseFilter_SetBit(void* Obj)
	{
		((FGameplayTargetDataFilter*)Obj)->bReverseFilter = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_bReverseFilter = { "bReverseFilter", nullptr, (EPropertyFlags)0x0011000000000005, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FGameplayTargetDataFilter), &Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_bReverseFilter_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_bReverseFilter_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_bReverseFilter_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_RequiredActorClass_MetaData[] = {
		{ "Category", "Filter" },
		{ "ExposeOnSpawn", "TRUE" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetDataFilter.h" },
		{ "ToolTip", "Subclass actors must be to pass the filter." },
	};
#endif
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_RequiredActorClass = { "RequiredActorClass", nullptr, (EPropertyFlags)0x0015000000000005, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTargetDataFilter, RequiredActorClass), Z_Construct_UClass_AActor_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_RequiredActorClass_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_RequiredActorClass_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfFilter_MetaData[] = {
		{ "Category", "Filter" },
		{ "ExposeOnSpawn", "TRUE" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetDataFilter.h" },
		{ "ToolTip", "Filter based on whether or not this actor is \"self.\"" },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfFilter = { "SelfFilter", nullptr, (EPropertyFlags)0x0011000000000005, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTargetDataFilter, SelfFilter), Z_Construct_UEnum_GameplayAbilities_ETargetDataFilterSelf, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfFilter_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfFilter_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfActor_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetDataFilter.h" },
		{ "ToolTip", "Actor we're comparing against." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfActor = { "SelfActor", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayTargetDataFilter, SelfActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfActor_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfActor_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_bReverseFilter,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_RequiredActorClass,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfFilter,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::NewProp_SelfActor,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayTargetDataFilter",
		sizeof(FGameplayTargetDataFilter),
		alignof(FGameplayTargetDataFilter),
		Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayTargetDataFilter()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayTargetDataFilter"), sizeof(FGameplayTargetDataFilter), Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayTargetDataFilter_Hash() { return 481011029U; }
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
