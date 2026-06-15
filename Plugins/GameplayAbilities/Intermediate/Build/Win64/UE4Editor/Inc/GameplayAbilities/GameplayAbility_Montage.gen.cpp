// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbility_Montage.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbility_Montage() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_Montage_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_Montage();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	ENGINE_API UClass* Z_Construct_UClass_UAnimMontage_NoRegister();
// End Cross Module References
	void UGameplayAbility_Montage::StaticRegisterNativesUGameplayAbility_Montage()
	{
	}
	UClass* Z_Construct_UClass_UGameplayAbility_Montage_NoRegister()
	{
		return UGameplayAbility_Montage::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayAbility_Montage_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayEffectsWhileAnimating_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_GameplayEffectsWhileAnimating;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_GameplayEffectsWhileAnimating_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayEffectClassesWhileAnimating_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_GameplayEffectClassesWhileAnimating;
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_GameplayEffectClassesWhileAnimating_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SectionName_MetaData[];
#endif
		static const UE4CodeGen_Private::FNamePropertyParams NewProp_SectionName;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PlayRate_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_PlayRate;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MontageToPlay_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MontageToPlay;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayAbility_Montage_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayAbility,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbility_Montage_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/GameplayAbility_Montage.h" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbility_Montage.h" },
		{ "ToolTip", "A gameplay ability that plays a single montage and applies a GameplayEffect" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectsWhileAnimating_MetaData[] = {
		{ "Category", "Deprecated" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbility_Montage.h" },
		{ "NativeConstTemplateArg", "" },
		{ "ToolTip", "Deprecated. Use GameplayEffectClassesWhileAnimating instead." },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectsWhileAnimating = { "GameplayEffectsWhileAnimating", nullptr, (EPropertyFlags)0x0010000000030001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbility_Montage, GameplayEffectsWhileAnimating), METADATA_PARAMS(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectsWhileAnimating_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectsWhileAnimating_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectsWhileAnimating_Inner = { "GameplayEffectsWhileAnimating", nullptr, (EPropertyFlags)0x0000000000020000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectClassesWhileAnimating_MetaData[] = {
		{ "Category", "MontageAbility" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbility_Montage.h" },
		{ "ToolTip", "GameplayEffects to apply and then remove while the animation is playing" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectClassesWhileAnimating = { "GameplayEffectClassesWhileAnimating", nullptr, (EPropertyFlags)0x0014000000010001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbility_Montage, GameplayEffectClassesWhileAnimating), METADATA_PARAMS(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectClassesWhileAnimating_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectClassesWhileAnimating_MetaData)) };
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectClassesWhileAnimating_Inner = { "GameplayEffectClassesWhileAnimating", nullptr, (EPropertyFlags)0x0004000000000000, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UGameplayEffect_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_SectionName_MetaData[] = {
		{ "Category", "MontageAbility" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbility_Montage.h" },
	};
#endif
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_SectionName = { "SectionName", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbility_Montage, SectionName), METADATA_PARAMS(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_SectionName_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_SectionName_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_PlayRate_MetaData[] = {
		{ "Category", "MontageAbility" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbility_Montage.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_PlayRate = { "PlayRate", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbility_Montage, PlayRate), METADATA_PARAMS(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_PlayRate_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_PlayRate_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_MontageToPlay_MetaData[] = {
		{ "Category", "MontageAbility" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbility_Montage.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_MontageToPlay = { "MontageToPlay", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayAbility_Montage, MontageToPlay), Z_Construct_UClass_UAnimMontage_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_MontageToPlay_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_MontageToPlay_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayAbility_Montage_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectsWhileAnimating,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectsWhileAnimating_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectClassesWhileAnimating,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_GameplayEffectClassesWhileAnimating_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_SectionName,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_PlayRate,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayAbility_Montage_Statics::NewProp_MontageToPlay,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayAbility_Montage_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayAbility_Montage>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayAbility_Montage_Statics::ClassParams = {
		&UGameplayAbility_Montage::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UGameplayAbility_Montage_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_Montage_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayAbility_Montage_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_Montage_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayAbility_Montage()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayAbility_Montage_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayAbility_Montage, 4279869615);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayAbility_Montage>()
	{
		return UGameplayAbility_Montage::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayAbility_Montage(Z_Construct_UClass_UGameplayAbility_Montage, &UGameplayAbility_Montage::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayAbility_Montage"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayAbility_Montage);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
