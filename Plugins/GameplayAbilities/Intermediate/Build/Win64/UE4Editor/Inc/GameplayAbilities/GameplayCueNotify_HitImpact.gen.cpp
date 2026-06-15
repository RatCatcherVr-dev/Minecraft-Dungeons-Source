// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCueNotify_HitImpact.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCueNotify_HitImpact() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueNotify_HitImpact_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueNotify_HitImpact();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueNotify_Static();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	ENGINE_API UClass* Z_Construct_UClass_UParticleSystem_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_USoundBase_NoRegister();
// End Cross Module References
	void UGameplayCueNotify_HitImpact::StaticRegisterNativesUGameplayCueNotify_HitImpact()
	{
	}
	UClass* Z_Construct_UClass_UGameplayCueNotify_HitImpact_NoRegister()
	{
		return UGameplayCueNotify_HitImpact::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ParticleSystem_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ParticleSystem;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Sound_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Sound;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayCueNotify_Static,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Category", "GameplayCue" },
		{ "DisplayName", "GCN Hit Impact (Deprecated)" },
		{ "HideCategories", "Replication" },
		{ "IncludePath", "GameplayCueNotify_HitImpact.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_HitImpact.h" },
		{ "ShortTooltip", "This class is deprecated. Use UFortGameplayCueNotify_Burst (GCN Burst) instead." },
		{ "ToolTip", "Non instanced GameplayCueNotify for spawning particle and sound FX.\nStill WIP - needs to be fleshed out more." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_ParticleSystem_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_HitImpact.h" },
		{ "ToolTip", "Effects to play for weapon attacks against specific surfaces" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_ParticleSystem = { "ParticleSystem", nullptr, (EPropertyFlags)0x0010000000000005, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueNotify_HitImpact, ParticleSystem), Z_Construct_UClass_UParticleSystem_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_ParticleSystem_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_ParticleSystem_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_Sound_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_HitImpact.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_Sound = { "Sound", nullptr, (EPropertyFlags)0x0010000000000005, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueNotify_HitImpact, Sound), Z_Construct_UClass_USoundBase_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_Sound_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_Sound_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_ParticleSystem,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::NewProp_Sound,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCueNotify_HitImpact>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::ClassParams = {
		&UGameplayCueNotify_HitImpact::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayCueNotify_HitImpact()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayCueNotify_HitImpact_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayCueNotify_HitImpact, 1931048481);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayCueNotify_HitImpact>()
	{
		return UGameplayCueNotify_HitImpact::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayCueNotify_HitImpact(Z_Construct_UClass_UGameplayCueNotify_HitImpact, &UGameplayCueNotify_HitImpact::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayCueNotify_HitImpact"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCueNotify_HitImpact);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
