// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityWorldReticle_ActorVisualization.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilityWorldReticle_ActorVisualization() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	ENGINE_API UClass* Z_Construct_UClass_UActorComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UCapsuleComponent_NoRegister();
// End Cross Module References
	void AGameplayAbilityWorldReticle_ActorVisualization::StaticRegisterNativesAGameplayAbilityWorldReticle_ActorVisualization()
	{
	}
	UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_NoRegister()
	{
		return AGameplayAbilityWorldReticle_ActorVisualization::StaticClass();
	}
	struct Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_VisualizationComponents_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_VisualizationComponents;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_VisualizationComponents_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CollisionComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_CollisionComponent;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameplayAbilityWorldReticle,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/GameplayAbilityWorldReticle_ActorVisualization.h" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle_ActorVisualization.h" },
		{ "ToolTip", "This is a dummy reticle for internal use by visualization placement tasks. It builds a custom visual model of the visualization being placed." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_VisualizationComponents_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle_ActorVisualization.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_VisualizationComponents = { "VisualizationComponents", nullptr, (EPropertyFlags)0x0010008000000008, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayAbilityWorldReticle_ActorVisualization, VisualizationComponents), METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_VisualizationComponents_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_VisualizationComponents_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_VisualizationComponents_Inner = { "VisualizationComponents", nullptr, (EPropertyFlags)0x0000000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UActorComponent_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_CollisionComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle_ActorVisualization.h" },
		{ "ToolTip", "Hardcoded collision component, so other objects don't think they can collide with the visualization actor" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_CollisionComponent = { "CollisionComponent", nullptr, (EPropertyFlags)0x0040000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayAbilityWorldReticle_ActorVisualization, CollisionComponent), Z_Construct_UClass_UCapsuleComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_CollisionComponent_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_CollisionComponent_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_VisualizationComponents,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_VisualizationComponents_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::NewProp_CollisionComponent,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameplayAbilityWorldReticle_ActorVisualization>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::ClassParams = {
		&AGameplayAbilityWorldReticle_ActorVisualization::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::PropPointers),
		0,
		0x008002A0u,
		METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGameplayAbilityWorldReticle_ActorVisualization, 781529374);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<AGameplayAbilityWorldReticle_ActorVisualization>()
	{
		return AGameplayAbilityWorldReticle_ActorVisualization::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGameplayAbilityWorldReticle_ActorVisualization(Z_Construct_UClass_AGameplayAbilityWorldReticle_ActorVisualization, &AGameplayAbilityWorldReticle_ActorVisualization::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("AGameplayAbilityWorldReticle_ActorVisualization"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGameplayAbilityWorldReticle_ActorVisualization);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
