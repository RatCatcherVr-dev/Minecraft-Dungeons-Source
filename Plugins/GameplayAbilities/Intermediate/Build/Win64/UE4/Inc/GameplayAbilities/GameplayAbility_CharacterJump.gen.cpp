// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbility_CharacterJump.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbility_CharacterJump() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_CharacterJump_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_CharacterJump();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References
	void UGameplayAbility_CharacterJump::StaticRegisterNativesUGameplayAbility_CharacterJump()
	{
	}
	UClass* Z_Construct_UClass_UGameplayAbility_CharacterJump_NoRegister()
	{
		return UGameplayAbility_CharacterJump::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayAbility,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/GameplayAbility_CharacterJump.h" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbility_CharacterJump.h" },
		{ "ToolTip", "Ability that jumps with a character." },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayAbility_CharacterJump>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics::ClassParams = {
		&UGameplayAbility_CharacterJump::StaticClass,
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
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayAbility_CharacterJump()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayAbility_CharacterJump_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayAbility_CharacterJump, 201977627);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayAbility_CharacterJump>()
	{
		return UGameplayAbility_CharacterJump::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayAbility_CharacterJump(Z_Construct_UClass_UGameplayAbility_CharacterJump, &UGameplayAbility_CharacterJump::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayAbility_CharacterJump"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayAbility_CharacterJump);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
