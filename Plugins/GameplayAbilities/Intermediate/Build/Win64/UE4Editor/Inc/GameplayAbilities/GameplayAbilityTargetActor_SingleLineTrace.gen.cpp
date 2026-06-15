// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityTargetActor_SingleLineTrace.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilityTargetActor_SingleLineTrace() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_Trace();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References
	void AGameplayAbilityTargetActor_SingleLineTrace::StaticRegisterNativesAGameplayAbilityTargetActor_SingleLineTrace()
	{
	}
	UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_NoRegister()
	{
		return AGameplayAbilityTargetActor_SingleLineTrace::StaticClass();
	}
	struct Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameplayAbilityTargetActor_Trace,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "Abilities/GameplayAbilityTargetActor_SingleLineTrace.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetActor_SingleLineTrace.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameplayAbilityTargetActor_SingleLineTrace>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics::ClassParams = {
		&AGameplayAbilityTargetActor_SingleLineTrace::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002A4u,
		METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGameplayAbilityTargetActor_SingleLineTrace, 3870494497);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<AGameplayAbilityTargetActor_SingleLineTrace>()
	{
		return AGameplayAbilityTargetActor_SingleLineTrace::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGameplayAbilityTargetActor_SingleLineTrace(Z_Construct_UClass_AGameplayAbilityTargetActor_SingleLineTrace, &AGameplayAbilityTargetActor_SingleLineTrace::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("AGameplayAbilityTargetActor_SingleLineTrace"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGameplayAbilityTargetActor_SingleLineTrace);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
