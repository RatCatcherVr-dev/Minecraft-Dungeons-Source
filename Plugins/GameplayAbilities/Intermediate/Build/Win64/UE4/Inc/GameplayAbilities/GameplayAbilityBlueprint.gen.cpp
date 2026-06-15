// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayAbilityBlueprint.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilityBlueprint() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbilityBlueprint_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbilityBlueprint();
	ENGINE_API UClass* Z_Construct_UClass_UBlueprint();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References
	void UGameplayAbilityBlueprint::StaticRegisterNativesUGameplayAbilityBlueprint()
	{
	}
	UClass* Z_Construct_UClass_UGameplayAbilityBlueprint_NoRegister()
	{
		return UGameplayAbilityBlueprint::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayAbilityBlueprint_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayAbilityBlueprint_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UBlueprint,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbilityBlueprint_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayAbilityBlueprint.h" },
		{ "ModuleRelativePath", "Public/GameplayAbilityBlueprint.h" },
		{ "ToolTip", "A Gameplay Ability Blueprint is essentially a specialized Blueprint whose graphs control a gameplay ability\nThe ability factory should pick this for you automatically" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayAbilityBlueprint_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayAbilityBlueprint>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayAbilityBlueprint_Statics::ClassParams = {
		&UGameplayAbilityBlueprint::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayAbilityBlueprint_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilityBlueprint_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayAbilityBlueprint()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayAbilityBlueprint_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayAbilityBlueprint, 3154500793);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayAbilityBlueprint>()
	{
		return UGameplayAbilityBlueprint::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayAbilityBlueprint(Z_Construct_UClass_UGameplayAbilityBlueprint, &UGameplayAbilityBlueprint::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayAbilityBlueprint"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayAbilityBlueprint);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
