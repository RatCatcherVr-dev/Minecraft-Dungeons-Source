// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayAbilitySet.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilitySet() {}
// Cross Module References
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityInputBinds();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilityBindInfo();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbilitySet_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbilitySet();
	ENGINE_API UClass* Z_Construct_UClass_UDataAsset();
// End Cross Module References
	static UEnum* EGameplayAbilityInputBinds_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityInputBinds, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("EGameplayAbilityInputBinds"));
		}
		return Singleton;
	}
	template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EGameplayAbilityInputBinds::Type>()
	{
		return EGameplayAbilityInputBinds_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EGameplayAbilityInputBinds(EGameplayAbilityInputBinds_StaticEnum, TEXT("/Script/GameplayAbilities"), TEXT("EGameplayAbilityInputBinds"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityInputBinds_Hash() { return 1013515332U; }
	UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityInputBinds()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EGameplayAbilityInputBinds"), 0, Get_Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityInputBinds_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "EGameplayAbilityInputBinds::Ability1", (int64)EGameplayAbilityInputBinds::Ability1 },
				{ "EGameplayAbilityInputBinds::Ability2", (int64)EGameplayAbilityInputBinds::Ability2 },
				{ "EGameplayAbilityInputBinds::Ability3", (int64)EGameplayAbilityInputBinds::Ability3 },
				{ "EGameplayAbilityInputBinds::Ability4", (int64)EGameplayAbilityInputBinds::Ability4 },
				{ "EGameplayAbilityInputBinds::Ability5", (int64)EGameplayAbilityInputBinds::Ability5 },
				{ "EGameplayAbilityInputBinds::Ability6", (int64)EGameplayAbilityInputBinds::Ability6 },
				{ "EGameplayAbilityInputBinds::Ability7", (int64)EGameplayAbilityInputBinds::Ability7 },
				{ "EGameplayAbilityInputBinds::Ability8", (int64)EGameplayAbilityInputBinds::Ability8 },
				{ "EGameplayAbilityInputBinds::Ability9", (int64)EGameplayAbilityInputBinds::Ability9 },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "Ability1.DisplayName", "Ability1 (LMB)" },
				{ "Ability2.DisplayName", "Ability2 (RMB)" },
				{ "Ability3.DisplayName", "Ability3 (Q)" },
				{ "Ability4.DisplayName", "Ability4 (E)" },
				{ "Ability5.DisplayName", "Ability5 (R)" },
				{ "Ability6.DisplayName", "Ability6 (Shift)" },
				{ "Ability7.DisplayName", "Ability7 (Space)" },
				{ "Ability8.DisplayName", "Ability8 (B)" },
				{ "Ability9.DisplayName", "Ability9 (T)" },
				{ "BlueprintType", "true" },
				{ "ModuleRelativePath", "Public/GameplayAbilitySet.h" },
				{ "ToolTip", "This is an example input binding enum for GameplayAbilities. Your project may want to create its own.\nThe MetaData default bind keys (LMB, RMB, Q, E, etc) are a convenience for designers setting up abilities sets\nor whatever other data you have that gives an ability with a default key binding. Actual key binding is up to DefaultInput.ini\n\nE.g., \"Ability1\" is the command string that is bound to AbilitySystemComponent::ActivateAbility(1). The Meta data only *suggests*\nthat you are binding \"Ability1\" to LMB by default in your projects DefaultInput.ini." },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
				nullptr,
				"EGameplayAbilityInputBinds",
				"EGameplayAbilityInputBinds::Type",
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
class UScriptStruct* FGameplayAbilityBindInfo::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAbilityBindInfo"), sizeof(FGameplayAbilityBindInfo), Get_Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAbilityBindInfo>()
{
	return FGameplayAbilityBindInfo::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAbilityBindInfo(FGameplayAbilityBindInfo::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAbilityBindInfo"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilityBindInfo
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilityBindInfo()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAbilityBindInfo")),new UScriptStruct::TCppStructOps<FGameplayAbilityBindInfo>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilityBindInfo;
	struct Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayAbilityClass_MetaData[];
#endif
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_GameplayAbilityClass;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Command_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_Command;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySet.h" },
		{ "ToolTip", "Example struct that pairs a enum input command to a GameplayAbilityClass.6" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAbilityBindInfo>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_GameplayAbilityClass_MetaData[] = {
		{ "Category", "BindInfo" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySet.h" },
	};
#endif
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_GameplayAbilityClass = { "GameplayAbilityClass", nullptr, (EPropertyFlags)0x0014000000000001, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilityBindInfo, GameplayAbilityClass), Z_Construct_UClass_UGameplayAbility_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_GameplayAbilityClass_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_GameplayAbilityClass_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_Command_MetaData[] = {
		{ "Category", "BindInfo" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySet.h" },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_Command = { "Command", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilityBindInfo, Command), Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityInputBinds, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_Command_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_Command_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_GameplayAbilityClass,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::NewProp_Command,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayAbilityBindInfo",
		sizeof(FGameplayAbilityBindInfo),
		alignof(FGameplayAbilityBindInfo),
		Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilityBindInfo()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAbilityBindInfo"), sizeof(FGameplayAbilityBindInfo), Get_Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilityBindInfo_Hash() { return 3257843461U; }
	void UGameplayAbilitySet::StaticRegisterNativesUGameplayAbilitySet()
	{
	}
	UClass* Z_Construct_UClass_UGameplayAbilitySet_NoRegister()
	{
		return UGameplayAbilitySet::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayAbilitySet_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Abilities_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Abilities;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Abilities_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayAbilitySet_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UDataAsset,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbilitySet_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySet.h" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySet.h" },
		{ "ToolTip", "This is an example DataAsset that could be used for defining a set of abilities to give to an AbilitySystemComponent and bind to an input command.\nYour project is free to implement this however it wants!" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbilitySet_Statics::NewProp_Abilities_MetaData[] = {
		{ "Category", "AbilitySet" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySet.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayAbilitySet_Statics::NewProp_Abilities = { "Abilities", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbilitySet, Abilities), METADATA_PARAMS(Z_Construct_UClass_UGameplayAbilitySet_Statics::NewProp_Abilities_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilitySet_Statics::NewProp_Abilities_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayAbilitySet_Statics::NewProp_Abilities_Inner = { "Abilities", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilityBindInfo, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayAbilitySet_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbilitySet_Statics::NewProp_Abilities,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbilitySet_Statics::NewProp_Abilities_Inner,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayAbilitySet_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayAbilitySet>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayAbilitySet_Statics::ClassParams = {
		&UGameplayAbilitySet::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayAbilitySet_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilitySet_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayAbilitySet_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilitySet_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayAbilitySet()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayAbilitySet_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayAbilitySet, 3551218602);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayAbilitySet>()
	{
		return UGameplayAbilitySet::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayAbilitySet(Z_Construct_UClass_UGameplayAbilitySet, &UGameplayAbilitySet::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayAbilitySet"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayAbilitySet);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
