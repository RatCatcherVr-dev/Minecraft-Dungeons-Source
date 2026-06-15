// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Source/Private/DungeonsNetConnection.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDungeonsNetConnection() {}
// Cross Module References
	ONLINESUBSYSTEMDUNGEONS_API UClass* Z_Construct_UClass_UDungeonsNetConnection_NoRegister();
	ONLINESUBSYSTEMDUNGEONS_API UClass* Z_Construct_UClass_UDungeonsNetConnection();
	ONLINESUBSYSTEMUTILS_API UClass* Z_Construct_UClass_UIpConnection();
	UPackage* Z_Construct_UPackage__Script_OnlineSubsystemDungeons();
// End Cross Module References
	void UDungeonsNetConnection::StaticRegisterNativesUDungeonsNetConnection()
	{
	}
	UClass* Z_Construct_UClass_UDungeonsNetConnection_NoRegister()
	{
		return UDungeonsNetConnection::StaticClass();
	}
	struct Z_Construct_UClass_UDungeonsNetConnection_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UDungeonsNetConnection_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UIpConnection,
		(UObject* (*)())Z_Construct_UPackage__Script_OnlineSubsystemDungeons,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDungeonsNetConnection_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "DungeonsNetConnection.h" },
		{ "ModuleRelativePath", "Private/DungeonsNetConnection.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UDungeonsNetConnection_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDungeonsNetConnection>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UDungeonsNetConnection_Statics::ClassParams = {
		&UDungeonsNetConnection::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_UDungeonsNetConnection_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UDungeonsNetConnection_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UDungeonsNetConnection()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UDungeonsNetConnection_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UDungeonsNetConnection, 1683616412);
	template<> ONLINESUBSYSTEMDUNGEONS_API UClass* StaticClass<UDungeonsNetConnection>()
	{
		return UDungeonsNetConnection::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UDungeonsNetConnection(Z_Construct_UClass_UDungeonsNetConnection, &UDungeonsNetConnection::StaticClass, TEXT("/Script/OnlineSubsystemDungeons"), TEXT("UDungeonsNetConnection"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UDungeonsNetConnection);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
