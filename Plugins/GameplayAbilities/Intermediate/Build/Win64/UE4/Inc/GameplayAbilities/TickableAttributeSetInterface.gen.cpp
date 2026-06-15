// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/TickableAttributeSetInterface.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTickableAttributeSetInterface() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UTickableAttributeSetInterface_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UTickableAttributeSetInterface();
	COREUOBJECT_API UClass* Z_Construct_UClass_UInterface();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References
	void UTickableAttributeSetInterface::StaticRegisterNativesUTickableAttributeSetInterface()
	{
	}
	UClass* Z_Construct_UClass_UTickableAttributeSetInterface_NoRegister()
	{
		return UTickableAttributeSetInterface::StaticClass();
	}
	struct Z_Construct_UClass_UTickableAttributeSetInterface_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UTickableAttributeSetInterface_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UInterface,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UTickableAttributeSetInterface_Statics::Class_MetaDataParams[] = {
		{ "CannotImplementInterfaceInBlueprint", "" },
		{ "ModuleRelativePath", "Public/TickableAttributeSetInterface.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UTickableAttributeSetInterface_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ITickableAttributeSetInterface>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UTickableAttributeSetInterface_Statics::ClassParams = {
		&UTickableAttributeSetInterface::StaticClass,
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
		0x000840A1u,
		METADATA_PARAMS(Z_Construct_UClass_UTickableAttributeSetInterface_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UTickableAttributeSetInterface_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UTickableAttributeSetInterface()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UTickableAttributeSetInterface_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UTickableAttributeSetInterface, 2528103066);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UTickableAttributeSetInterface>()
	{
		return UTickableAttributeSetInterface::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UTickableAttributeSetInterface(Z_Construct_UClass_UTickableAttributeSetInterface, &UTickableAttributeSetInterface::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UTickableAttributeSetInterface"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UTickableAttributeSetInterface);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
