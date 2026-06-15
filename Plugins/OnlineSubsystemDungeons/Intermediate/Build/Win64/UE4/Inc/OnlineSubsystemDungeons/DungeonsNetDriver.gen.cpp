// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Source/Private/DungeonsNetDriver.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDungeonsNetDriver() {}
// Cross Module References
	ONLINESUBSYSTEMDUNGEONS_API UClass* Z_Construct_UClass_UDungeonsNetDriver_NoRegister();
	ONLINESUBSYSTEMDUNGEONS_API UClass* Z_Construct_UClass_UDungeonsNetDriver();
	ONLINESUBSYSTEMUTILS_API UClass* Z_Construct_UClass_UIpNetDriver();
	UPackage* Z_Construct_UPackage__Script_OnlineSubsystemDungeons();
// End Cross Module References
	void UDungeonsNetDriver::StaticRegisterNativesUDungeonsNetDriver()
	{
	}
	UClass* Z_Construct_UClass_UDungeonsNetDriver_NoRegister()
	{
		return UDungeonsNetDriver::StaticClass();
	}
	struct Z_Construct_UClass_UDungeonsNetDriver_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UDungeonsNetDriver_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UIpNetDriver,
		(UObject* (*)())Z_Construct_UPackage__Script_OnlineSubsystemDungeons,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDungeonsNetDriver_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "DungeonsNetDriver.h" },
		{ "ModuleRelativePath", "Private/DungeonsNetDriver.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UDungeonsNetDriver_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDungeonsNetDriver>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UDungeonsNetDriver_Statics::ClassParams = {
		&UDungeonsNetDriver::StaticClass,
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
		0x000000ACu,
		METADATA_PARAMS(Z_Construct_UClass_UDungeonsNetDriver_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UDungeonsNetDriver_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UDungeonsNetDriver()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UDungeonsNetDriver_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UDungeonsNetDriver, 1135075971);
	template<> ONLINESUBSYSTEMDUNGEONS_API UClass* StaticClass<UDungeonsNetDriver>()
	{
		return UDungeonsNetDriver::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UDungeonsNetDriver(Z_Construct_UClass_UDungeonsNetDriver, &UDungeonsNetDriver::StaticClass, TEXT("/Script/OnlineSubsystemDungeons"), TEXT("UDungeonsNetDriver"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UDungeonsNetDriver);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
