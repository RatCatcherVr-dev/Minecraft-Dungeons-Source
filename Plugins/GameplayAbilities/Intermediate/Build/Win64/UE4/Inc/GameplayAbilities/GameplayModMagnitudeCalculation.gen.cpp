// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayModMagnitudeCalculation.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayModMagnitudeCalculation() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayModMagnitudeCalculation_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayModMagnitudeCalculation();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectCalculation();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectSpec();
// End Cross Module References
	static FName NAME_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude = FName(TEXT("CalculateBaseMagnitude"));
	float UGameplayModMagnitudeCalculation::CalculateBaseMagnitude(FGameplayEffectSpec const& Spec) const
	{
		GameplayModMagnitudeCalculation_eventCalculateBaseMagnitude_Parms Parms;
		Parms.Spec=Spec;
		const_cast<UGameplayModMagnitudeCalculation*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude),&Parms);
		return Parms.ReturnValue;
	}
	void UGameplayModMagnitudeCalculation::StaticRegisterNativesUGameplayModMagnitudeCalculation()
	{
		UClass* Class = UGameplayModMagnitudeCalculation::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "CalculateBaseMagnitude", &UGameplayModMagnitudeCalculation::execCalculateBaseMagnitude },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics
	{
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ReturnValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Spec_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Spec;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayModMagnitudeCalculation_eventCalculateBaseMagnitude_Parms, ReturnValue), METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::NewProp_Spec_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::NewProp_Spec = { "Spec", nullptr, (EPropertyFlags)0x0010000008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayModMagnitudeCalculation_eventCalculateBaseMagnitude_Parms, Spec), Z_Construct_UScriptStruct_FGameplayEffectSpec, METADATA_PARAMS(Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::NewProp_Spec_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::NewProp_Spec_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::NewProp_Spec,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::Function_MetaDataParams[] = {
		{ "Category", "Calculation" },
		{ "ModuleRelativePath", "Public/GameplayModMagnitudeCalculation.h" },
		{ "ToolTip", "Calculate the base magnitude of the gameplay effect modifier, given the specified spec. Note that the owning spec def can still modify this base value\nwith a coeffecient and pre/post multiply adds. see FCustomCalculationBasedFloat::CalculateMagnitude for details.\n\n@param Spec  Gameplay effect spec to use to calculate the magnitude with\n\n@return Computed magnitude of the modifier" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayModMagnitudeCalculation, nullptr, "CalculateBaseMagnitude", sizeof(GameplayModMagnitudeCalculation_eventCalculateBaseMagnitude_Parms), Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x48420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UGameplayModMagnitudeCalculation_NoRegister()
	{
		return UGameplayModMagnitudeCalculation::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bAllowNonNetAuthorityDependencyRegistration_MetaData[];
#endif
		static void NewProp_bAllowNonNetAuthorityDependencyRegistration_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bAllowNonNetAuthorityDependencyRegistration;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayEffectCalculation,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayModMagnitudeCalculation_CalculateBaseMagnitude, "CalculateBaseMagnitude" }, // 3165994955
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayModMagnitudeCalculation.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/GameplayModMagnitudeCalculation.h" },
		{ "ToolTip", "Class used to perform custom gameplay effect modifier calculations, either via blueprint or native code" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::NewProp_bAllowNonNetAuthorityDependencyRegistration_MetaData[] = {
		{ "Category", "ExternalDependencies" },
		{ "ModuleRelativePath", "Public/GameplayModMagnitudeCalculation.h" },
		{ "ToolTip", "Whether the calculation allows non-net authorities to register the external dependency multi-cast delegate or not; Effectively\nwhether clients are allowed to perform the custom calculation themselves or not\n\n@Note:       This is an advanced use case that should only be enabled under very specific circumstances. This is designed for games\n                     that are utilizing network dormancy and may want to trust the client to update non-gameplay critical attributes client-side without\n                     causing a network dormancy flush. Usage of this flag is *NOT* compatible with attribute capture within the calculation and will trigger\n                     an ensure if used in tandem. Clients are incapable of performing custom calculations that require attribute captures. In general,\n                     if your game is not using network dormancy, this should always remain disabled.\n\n@Note:       If enabling this for a custom calculation, be sure that the external delegate is sourced from something guaranteed to be on the client to avoid\n                     timing issues. As an example, binding to a delegate on a GameState is potentially unreliable, as the client reference to that actor may not be\n                     available during registration." },
	};
#endif
	void Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::NewProp_bAllowNonNetAuthorityDependencyRegistration_SetBit(void* Obj)
	{
		((UGameplayModMagnitudeCalculation*)Obj)->bAllowNonNetAuthorityDependencyRegistration = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::NewProp_bAllowNonNetAuthorityDependencyRegistration = { "bAllowNonNetAuthorityDependencyRegistration", nullptr, (EPropertyFlags)0x00200c0000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UGameplayModMagnitudeCalculation), &Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::NewProp_bAllowNonNetAuthorityDependencyRegistration_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::NewProp_bAllowNonNetAuthorityDependencyRegistration_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::NewProp_bAllowNonNetAuthorityDependencyRegistration_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::NewProp_bAllowNonNetAuthorityDependencyRegistration,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayModMagnitudeCalculation>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::ClassParams = {
		&UGameplayModMagnitudeCalculation::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::PropPointers),
		0,
		0x001000A1u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayModMagnitudeCalculation()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayModMagnitudeCalculation_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayModMagnitudeCalculation, 2204445774);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayModMagnitudeCalculation>()
	{
		return UGameplayModMagnitudeCalculation::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayModMagnitudeCalculation(Z_Construct_UClass_UGameplayModMagnitudeCalculation, &UGameplayModMagnitudeCalculation::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayModMagnitudeCalculation"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayModMagnitudeCalculation);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
