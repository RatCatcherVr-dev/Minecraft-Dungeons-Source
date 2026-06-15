// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilitiesEditor/Public/GameplayAbilitiesBlueprintFactory.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilitiesBlueprintFactory() {}
// Cross Module References
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_NoRegister();
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory();
	UNREALED_API UClass* Z_Construct_UClass_UFactory();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilitiesEditor();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
	ENGINE_API UEnum* Z_Construct_UEnum_Engine_EBlueprintType();
// End Cross Module References
	void UGameplayAbilitiesBlueprintFactory::StaticRegisterNativesUGameplayAbilitiesBlueprintFactory()
	{
	}
	UClass* Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_NoRegister()
	{
		return UGameplayAbilitiesBlueprintFactory::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ParentClass_MetaData[];
#endif
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_ParentClass;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_BlueprintType_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_BlueprintType;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UFactory,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilitiesEditor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Object" },
		{ "IncludePath", "GameplayAbilitiesBlueprintFactory.h" },
		{ "ModuleRelativePath", "Public/GameplayAbilitiesBlueprintFactory.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_ParentClass_MetaData[] = {
		{ "Category", "GameplayAbilitiesBlueprintFactory" },
		{ "ModuleRelativePath", "Public/GameplayAbilitiesBlueprintFactory.h" },
		{ "ToolTip", "The parent class of the created blueprint" },
	};
#endif
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_ParentClass = { "ParentClass", nullptr, (EPropertyFlags)0x0014000000000001, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbilitiesBlueprintFactory, ParentClass), Z_Construct_UClass_UGameplayAbility_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_ParentClass_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_ParentClass_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_BlueprintType_MetaData[] = {
		{ "Category", "GameplayAbilitiesBlueprintFactory" },
		{ "ModuleRelativePath", "Public/GameplayAbilitiesBlueprintFactory.h" },
		{ "ToolTip", "The type of blueprint that will be created" },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_BlueprintType = { "BlueprintType", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbilitiesBlueprintFactory, BlueprintType), Z_Construct_UEnum_Engine_EBlueprintType, METADATA_PARAMS(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_BlueprintType_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_BlueprintType_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_ParentClass,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::NewProp_BlueprintType,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayAbilitiesBlueprintFactory>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::ClassParams = {
		&UGameplayAbilitiesBlueprintFactory::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::PropPointers),
		0,
		0x000800A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayAbilitiesBlueprintFactory, 470133115);
	template<> GAMEPLAYABILITIESEDITOR_API UClass* StaticClass<UGameplayAbilitiesBlueprintFactory>()
	{
		return UGameplayAbilitiesBlueprintFactory::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayAbilitiesBlueprintFactory(Z_Construct_UClass_UGameplayAbilitiesBlueprintFactory, &UGameplayAbilitiesBlueprintFactory::StaticClass, TEXT("/Script/GameplayAbilitiesEditor"), TEXT("UGameplayAbilitiesBlueprintFactory"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayAbilitiesBlueprintFactory);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
