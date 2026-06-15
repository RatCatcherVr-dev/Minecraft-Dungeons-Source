// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayEffectUIData_TextOnly.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayEffectUIData_TextOnly() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectUIData_TextOnly_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectUIData_TextOnly();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectUIData();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References
	void UGameplayEffectUIData_TextOnly::StaticRegisterNativesUGameplayEffectUIData_TextOnly()
	{
	}
	UClass* Z_Construct_UClass_UGameplayEffectUIData_TextOnly_NoRegister()
	{
		return UGameplayEffectUIData_TextOnly::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Description_MetaData[];
#endif
		static const UE4CodeGen_Private::FTextPropertyParams NewProp_Description;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayEffectUIData,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayEffectUIData_TextOnly.h" },
		{ "ModuleRelativePath", "Public/GameplayEffectUIData_TextOnly.h" },
		{ "ToolTip", "UI data that contains only text. This is mostly used as an example of a subclass of UGameplayEffectUIData.\nIf your game needs only text, this is a reasonable class to use. To include more data, make a custom subclass of UGameplayEffectUIData." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::NewProp_Description_MetaData[] = {
		{ "Category", "Data" },
		{ "ModuleRelativePath", "Public/GameplayEffectUIData_TextOnly.h" },
		{ "MultiLine", "true" },
	};
#endif
	const UE4CodeGen_Private::FTextPropertyParams Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::NewProp_Description = { "Description", nullptr, (EPropertyFlags)0x0010000000010015, UE4CodeGen_Private::EPropertyGenFlags::Text, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayEffectUIData_TextOnly, Description), METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::NewProp_Description_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::NewProp_Description_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::NewProp_Description,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayEffectUIData_TextOnly>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::ClassParams = {
		&UGameplayEffectUIData_TextOnly::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::PropPointers),
		0,
		0x001030A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayEffectUIData_TextOnly()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayEffectUIData_TextOnly_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayEffectUIData_TextOnly, 3083163516);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayEffectUIData_TextOnly>()
	{
		return UGameplayEffectUIData_TextOnly::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayEffectUIData_TextOnly(Z_Construct_UClass_UGameplayEffectUIData_TextOnly, &UGameplayEffectUIData_TextOnly::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayEffectUIData_TextOnly"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayEffectUIData_TextOnly);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
