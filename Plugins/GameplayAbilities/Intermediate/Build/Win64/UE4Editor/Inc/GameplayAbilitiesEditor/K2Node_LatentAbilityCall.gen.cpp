// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilitiesEditor/Public/K2Node_LatentAbilityCall.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeK2Node_LatentAbilityCall() {}
// Cross Module References
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UK2Node_LatentAbilityCall_NoRegister();
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UK2Node_LatentAbilityCall();
	GAMEPLAYTASKSEDITOR_API UClass* Z_Construct_UClass_UK2Node_LatentGameplayTaskCall();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilitiesEditor();
// End Cross Module References
	void UK2Node_LatentAbilityCall::StaticRegisterNativesUK2Node_LatentAbilityCall()
	{
	}
	UClass* Z_Construct_UClass_UK2Node_LatentAbilityCall_NoRegister()
	{
		return UK2Node_LatentAbilityCall::StaticClass();
	}
	struct Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UK2Node_LatentGameplayTaskCall,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilitiesEditor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "K2Node_LatentAbilityCall.h" },
		{ "ModuleRelativePath", "Public/K2Node_LatentAbilityCall.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UK2Node_LatentAbilityCall>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics::ClassParams = {
		&UK2Node_LatentAbilityCall::StaticClass,
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
		0x000000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UK2Node_LatentAbilityCall()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UK2Node_LatentAbilityCall_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UK2Node_LatentAbilityCall, 1153169476);
	template<> GAMEPLAYABILITIESEDITOR_API UClass* StaticClass<UK2Node_LatentAbilityCall>()
	{
		return UK2Node_LatentAbilityCall::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UK2Node_LatentAbilityCall(Z_Construct_UClass_UK2Node_LatentAbilityCall, &UK2Node_LatentAbilityCall::StaticClass, TEXT("/Script/GameplayAbilitiesEditor"), TEXT("UK2Node_LatentAbilityCall"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UK2Node_LatentAbilityCall);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
