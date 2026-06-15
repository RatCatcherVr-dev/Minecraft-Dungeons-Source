// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilitiesEditor/Public/GameplayEffectCreationMenu.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayEffectCreationMenu() {}
// Cross Module References
	GAMEPLAYABILITIESEDITOR_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectCreationData();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilitiesEditor();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UGameplayEffectCreationMenu_NoRegister();
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UGameplayEffectCreationMenu();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
// End Cross Module References
class UScriptStruct* FGameplayEffectCreationData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIESEDITOR_API uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCreationData_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayEffectCreationData, Z_Construct_UPackage__Script_GameplayAbilitiesEditor(), TEXT("GameplayEffectCreationData"), sizeof(FGameplayEffectCreationData), Get_Z_Construct_UScriptStruct_FGameplayEffectCreationData_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIESEDITOR_API UScriptStruct* StaticStruct<FGameplayEffectCreationData>()
{
	return FGameplayEffectCreationData::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayEffectCreationData(FGameplayEffectCreationData::StaticStruct, TEXT("/Script/GameplayAbilitiesEditor"), TEXT("GameplayEffectCreationData"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilitiesEditor_StaticRegisterNativesFGameplayEffectCreationData
{
	FScriptStruct_GameplayAbilitiesEditor_StaticRegisterNativesFGameplayEffectCreationData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayEffectCreationData")),new UScriptStruct::TCppStructOps<FGameplayEffectCreationData>);
	}
} ScriptStruct_GameplayAbilitiesEditor_StaticRegisterNativesFGameplayEffectCreationData;
	struct Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ParentGameplayEffect_MetaData[];
#endif
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_ParentGameplayEffect;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_BaseName_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_BaseName;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MenuPath_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_MenuPath;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayEffectCreationMenu.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayEffectCreationData>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_ParentGameplayEffect_MetaData[] = {
		{ "Category", "Gameplay Effect" },
		{ "ModuleRelativePath", "Public/GameplayEffectCreationMenu.h" },
	};
#endif
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_ParentGameplayEffect = { "ParentGameplayEffect", nullptr, (EPropertyFlags)0x0014000000000001, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayEffectCreationData, ParentGameplayEffect), Z_Construct_UClass_UGameplayEffect_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_ParentGameplayEffect_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_ParentGameplayEffect_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_BaseName_MetaData[] = {
		{ "Category", "Gameplay Effect" },
		{ "ModuleRelativePath", "Public/GameplayEffectCreationMenu.h" },
		{ "ToolTip", "The default BaseName of the new asset. E.g \"Damage\" -> GE_Damage or GE_HeroName_AbilityName_Damage" },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_BaseName = { "BaseName", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayEffectCreationData, BaseName), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_BaseName_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_BaseName_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_MenuPath_MetaData[] = {
		{ "Category", "Gameplay Effect" },
		{ "ModuleRelativePath", "Public/GameplayEffectCreationMenu.h" },
		{ "ToolTip", "Where to show this in the menu. Use \"|\" for sub categories. E.g, \"Status|Hard|Stun|Root\"." },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_MenuPath = { "MenuPath", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayEffectCreationData, MenuPath), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_MenuPath_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_MenuPath_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_ParentGameplayEffect,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_BaseName,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::NewProp_MenuPath,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilitiesEditor,
		nullptr,
		&NewStructOps,
		"GameplayEffectCreationData",
		sizeof(FGameplayEffectCreationData),
		alignof(FGameplayEffectCreationData),
		Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectCreationData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCreationData_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilitiesEditor();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayEffectCreationData"), sizeof(FGameplayEffectCreationData), Get_Z_Construct_UScriptStruct_FGameplayEffectCreationData_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayEffectCreationData_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCreationData_Hash() { return 3954767774U; }
	void UGameplayEffectCreationMenu::StaticRegisterNativesUGameplayEffectCreationMenu()
	{
	}
	UClass* Z_Construct_UClass_UGameplayEffectCreationMenu_NoRegister()
	{
		return UGameplayEffectCreationMenu::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayEffectCreationMenu_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Definitions_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Definitions;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Definitions_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilitiesEditor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayEffectCreationMenu.h" },
		{ "ModuleRelativePath", "Public/GameplayEffectCreationMenu.h" },
		{ "ToolTip", "Container to hold EventKeywords for PIE testing" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::NewProp_Definitions_MetaData[] = {
		{ "Category", "Gameplay Effect" },
		{ "ModuleRelativePath", "Public/GameplayEffectCreationMenu.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::NewProp_Definitions = { "Definitions", nullptr, (EPropertyFlags)0x0010000000004001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayEffectCreationMenu, Definitions), METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::NewProp_Definitions_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::NewProp_Definitions_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::NewProp_Definitions_Inner = { "Definitions", nullptr, (EPropertyFlags)0x0000000000004000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayEffectCreationData, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::NewProp_Definitions,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::NewProp_Definitions_Inner,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayEffectCreationMenu>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::ClassParams = {
		&UGameplayEffectCreationMenu::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::PropPointers),
		0,
		0x001002A6u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayEffectCreationMenu()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayEffectCreationMenu_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayEffectCreationMenu, 2018795773);
	template<> GAMEPLAYABILITIESEDITOR_API UClass* StaticClass<UGameplayEffectCreationMenu>()
	{
		return UGameplayEffectCreationMenu::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayEffectCreationMenu(Z_Construct_UClass_UGameplayEffectCreationMenu, &UGameplayEffectCreationMenu::StaticClass, TEXT("/Script/GameplayAbilitiesEditor"), TEXT("UGameplayEffectCreationMenu"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayEffectCreationMenu);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
