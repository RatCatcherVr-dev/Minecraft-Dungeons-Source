// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilityTargetActor_GroundTrace() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_Trace();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References
	void AGameplayAbilityTargetActor_GroundTrace::StaticRegisterNativesAGameplayAbilityTargetActor_GroundTrace()
	{
	}
	UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_NoRegister()
	{
		return AGameplayAbilityTargetActor_GroundTrace::StaticClass();
	}
	struct Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CollisionHeight_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_CollisionHeight;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CollisionRadius_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_CollisionRadius;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameplayAbilityTargetActor_Trace,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "Abilities/GameplayAbilityTargetActor_GroundTrace.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetActor_GroundTrace.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionHeight_MetaData[] = {
		{ "Category", "Targeting" },
		{ "ExposeOnSpawn", "TRUE" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetActor_GroundTrace.h" },
		{ "ToolTip", "Height for a capsule. Implicitly indicates a capsule is desired if this is greater than zero." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionHeight = { "CollisionHeight", nullptr, (EPropertyFlags)0x0011000000000005, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayAbilityTargetActor_GroundTrace, CollisionHeight), METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionHeight_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionHeight_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionRadius_MetaData[] = {
		{ "Category", "Targeting" },
		{ "ExposeOnSpawn", "TRUE" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityTargetActor_GroundTrace.h" },
		{ "ToolTip", "Radius for a sphere or capsule." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionRadius = { "CollisionRadius", nullptr, (EPropertyFlags)0x0011000000000005, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayAbilityTargetActor_GroundTrace, CollisionRadius), METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionRadius_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionRadius_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionHeight,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::NewProp_CollisionRadius,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameplayAbilityTargetActor_GroundTrace>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::ClassParams = {
		&AGameplayAbilityTargetActor_GroundTrace::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::PropPointers),
		0,
		0x009002A4u,
		METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGameplayAbilityTargetActor_GroundTrace, 3872167657);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<AGameplayAbilityTargetActor_GroundTrace>()
	{
		return AGameplayAbilityTargetActor_GroundTrace::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGameplayAbilityTargetActor_GroundTrace(Z_Construct_UClass_AGameplayAbilityTargetActor_GroundTrace, &AGameplayAbilityTargetActor_GroundTrace::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("AGameplayAbilityTargetActor_GroundTrace"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGameplayAbilityTargetActor_GroundTrace);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
