// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayEffectCustomApplicationRequirement.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayEffectCustomApplicationRequirement() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectSpec();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
// End Cross Module References
	static FName NAME_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect = FName(TEXT("CanApplyGameplayEffect"));
	bool UGameplayEffectCustomApplicationRequirement::CanApplyGameplayEffect(const UGameplayEffect* GameplayEffect, FGameplayEffectSpec const& Spec, UAbilitySystemComponent* ASC) const
	{
		GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms Parms;
		Parms.GameplayEffect=GameplayEffect;
		Parms.Spec=Spec;
		Parms.ASC=ASC;
		const_cast<UGameplayEffectCustomApplicationRequirement*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect),&Parms);
		return !!Parms.ReturnValue;
	}
	void UGameplayEffectCustomApplicationRequirement::StaticRegisterNativesUGameplayEffectCustomApplicationRequirement()
	{
		UClass* Class = UGameplayEffectCustomApplicationRequirement::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "CanApplyGameplayEffect", &UGameplayEffectCustomApplicationRequirement::execCanApplyGameplayEffect },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics
	{
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ASC_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ASC;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Spec_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Spec;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayEffect_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_GameplayEffect;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms), &Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ASC_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ASC = { "ASC", nullptr, (EPropertyFlags)0x0010000000080080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms, ASC), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ASC_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ASC_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_Spec_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_Spec = { "Spec", nullptr, (EPropertyFlags)0x0010000008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms, Spec), Z_Construct_UScriptStruct_FGameplayEffectSpec, METADATA_PARAMS(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_Spec_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_Spec_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_GameplayEffect_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_GameplayEffect = { "GameplayEffect", nullptr, (EPropertyFlags)0x0010000000000082, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms, GameplayEffect), Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_GameplayEffect_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_GameplayEffect_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_ASC,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_Spec,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::NewProp_GameplayEffect,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::Function_MetaDataParams[] = {
		{ "Category", "Calculation" },
		{ "ModuleRelativePath", "Public/GameplayEffectCustomApplicationRequirement.h" },
		{ "ToolTip", "Return whether the gameplay effect should be applied or not" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement, nullptr, "CanApplyGameplayEffect", sizeof(GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms), Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x48420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_NoRegister()
	{
		return UGameplayEffectCustomApplicationRequirement::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayEffectCustomApplicationRequirement_CanApplyGameplayEffect, "CanApplyGameplayEffect" }, // 3358515851
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayEffectCustomApplicationRequirement.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/GameplayEffectCustomApplicationRequirement.h" },
		{ "ToolTip", "Class used to perform custom gameplay effect modifier calculations, either via blueprint or native code" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayEffectCustomApplicationRequirement>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::ClassParams = {
		&UGameplayEffectCustomApplicationRequirement::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x001000A1u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayEffectCustomApplicationRequirement, 3516221023);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayEffectCustomApplicationRequirement>()
	{
		return UGameplayEffectCustomApplicationRequirement::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayEffectCustomApplicationRequirement(Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement, &UGameplayEffectCustomApplicationRequirement::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayEffectCustomApplicationRequirement"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayEffectCustomApplicationRequirement);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
