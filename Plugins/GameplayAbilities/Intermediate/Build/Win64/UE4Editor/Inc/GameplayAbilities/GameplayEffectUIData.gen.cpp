// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayEffectUIData.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayEffectUIData() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectUIData_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectUIData();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References
	void UGameplayEffectUIData::StaticRegisterNativesUGameplayEffectUIData()
	{
	}
	UClass* Z_Construct_UClass_UGameplayEffectUIData_NoRegister()
	{
		return UGameplayEffectUIData::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayEffectUIData_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayEffectUIData_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectUIData_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayEffectUIData.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/GameplayEffectUIData.h" },
		{ "ToolTip", "UGameplayEffectUIData\nBase class to provide game-specific data about how to describe a Gameplay Effect in the UI. Subclass with data to use in your game." },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayEffectUIData_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayEffectUIData>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayEffectUIData_Statics::ClassParams = {
		&UGameplayEffectUIData::StaticClass,
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
		0x001030A1u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectUIData_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectUIData_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayEffectUIData()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayEffectUIData_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayEffectUIData, 4073768782);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayEffectUIData>()
	{
		return UGameplayEffectUIData::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayEffectUIData(Z_Construct_UClass_UGameplayEffectUIData, &UGameplayEffectUIData::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayEffectUIData"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayEffectUIData);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
