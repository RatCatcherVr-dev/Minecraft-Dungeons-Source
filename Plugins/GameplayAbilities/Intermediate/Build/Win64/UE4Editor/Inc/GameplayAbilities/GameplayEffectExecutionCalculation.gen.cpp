// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayEffectExecutionCalculation.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayEffectExecutionCalculation() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayModifierEvaluatedData();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectExecutionCalculation_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectExecutionCalculation();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectCalculation();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectAttributeCaptureDefinition();
// End Cross Module References
class UScriptStruct* FGameplayEffectCustomExecutionOutput::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayEffectCustomExecutionOutput"), sizeof(FGameplayEffectCustomExecutionOutput), Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayEffectCustomExecutionOutput>()
{
	return FGameplayEffectCustomExecutionOutput::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayEffectCustomExecutionOutput(FGameplayEffectCustomExecutionOutput::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayEffectCustomExecutionOutput"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayEffectCustomExecutionOutput
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayEffectCustomExecutionOutput()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayEffectCustomExecutionOutput")),new UScriptStruct::TCppStructOps<FGameplayEffectCustomExecutionOutput>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayEffectCustomExecutionOutput;
	struct Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bHandledGameplayCuesManually_MetaData[];
#endif
		static void NewProp_bHandledGameplayCuesManually_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bHandledGameplayCuesManually;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bHandledStackCountManually_MetaData[];
#endif
		static void NewProp_bHandledStackCountManually_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bHandledStackCountManually;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bTriggerConditionalGameplayEffects_MetaData[];
#endif
		static void NewProp_bTriggerConditionalGameplayEffects_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bTriggerConditionalGameplayEffects;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OutputModifiers_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_OutputModifiers;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_OutputModifiers_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "Struct representing the output of a custom gameplay effect execution." },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayEffectCustomExecutionOutput>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledGameplayCuesManually_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "If true, the execution itself has manually invoked all gameplay cues and the GE system doesn't have to automatically handle them." },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledGameplayCuesManually_SetBit(void* Obj)
	{
		((FGameplayEffectCustomExecutionOutput*)Obj)->bHandledGameplayCuesManually = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledGameplayCuesManually = { "bHandledGameplayCuesManually", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool , RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(uint8), sizeof(FGameplayEffectCustomExecutionOutput), &Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledGameplayCuesManually_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledGameplayCuesManually_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledGameplayCuesManually_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledStackCountManually_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "If true, the execution itself has manually handled the stack count of the effect and the GE system doesn't have to automatically handle it" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledStackCountManually_SetBit(void* Obj)
	{
		((FGameplayEffectCustomExecutionOutput*)Obj)->bHandledStackCountManually = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledStackCountManually = { "bHandledStackCountManually", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool , RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(uint8), sizeof(FGameplayEffectCustomExecutionOutput), &Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledStackCountManually_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledStackCountManually_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledStackCountManually_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bTriggerConditionalGameplayEffects_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "If true, the execution wants to trigger conditional gameplay effects when it completes" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bTriggerConditionalGameplayEffects_SetBit(void* Obj)
	{
		((FGameplayEffectCustomExecutionOutput*)Obj)->bTriggerConditionalGameplayEffects = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bTriggerConditionalGameplayEffects = { "bTriggerConditionalGameplayEffects", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool , RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(uint8), sizeof(FGameplayEffectCustomExecutionOutput), &Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bTriggerConditionalGameplayEffects_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bTriggerConditionalGameplayEffects_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bTriggerConditionalGameplayEffects_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_OutputModifiers_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "Modifiers emitted by the execution" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_OutputModifiers = { "OutputModifiers", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayEffectCustomExecutionOutput, OutputModifiers), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_OutputModifiers_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_OutputModifiers_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_OutputModifiers_Inner = { "OutputModifiers", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayModifierEvaluatedData, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledGameplayCuesManually,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bHandledStackCountManually,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_bTriggerConditionalGameplayEffects,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_OutputModifiers,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::NewProp_OutputModifiers_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayEffectCustomExecutionOutput",
		sizeof(FGameplayEffectCustomExecutionOutput),
		alignof(FGameplayEffectCustomExecutionOutput),
		Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayEffectCustomExecutionOutput"), sizeof(FGameplayEffectCustomExecutionOutput), Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput_Hash() { return 4272328388U; }
class UScriptStruct* FGameplayEffectCustomExecutionParameters::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayEffectCustomExecutionParameters"), sizeof(FGameplayEffectCustomExecutionParameters), Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayEffectCustomExecutionParameters>()
{
	return FGameplayEffectCustomExecutionParameters::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayEffectCustomExecutionParameters(FGameplayEffectCustomExecutionParameters::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayEffectCustomExecutionParameters"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayEffectCustomExecutionParameters
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayEffectCustomExecutionParameters()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayEffectCustomExecutionParameters")),new UScriptStruct::TCppStructOps<FGameplayEffectCustomExecutionParameters>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayEffectCustomExecutionParameters;
	struct Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "Struct representing parameters for a custom gameplay effect execution. Should not be held onto via reference, used just for the scope of the execution" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayEffectCustomExecutionParameters>();
	}
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayEffectCustomExecutionParameters",
		sizeof(FGameplayEffectCustomExecutionParameters),
		alignof(FGameplayEffectCustomExecutionParameters),
		nullptr,
		0,
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayEffectCustomExecutionParameters"), sizeof(FGameplayEffectCustomExecutionParameters), Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters_Hash() { return 2105451208U; }
	static FName NAME_UGameplayEffectExecutionCalculation_Execute = FName(TEXT("Execute"));
	void UGameplayEffectExecutionCalculation::Execute(FGameplayEffectCustomExecutionParameters const& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
	{
		GameplayEffectExecutionCalculation_eventExecute_Parms Parms;
		Parms.ExecutionParams=ExecutionParams;
		Parms.OutExecutionOutput=OutExecutionOutput;
		const_cast<UGameplayEffectExecutionCalculation*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayEffectExecutionCalculation_Execute),&Parms);
		OutExecutionOutput=Parms.OutExecutionOutput;
	}
	void UGameplayEffectExecutionCalculation::StaticRegisterNativesUGameplayEffectExecutionCalculation()
	{
		UClass* Class = UGameplayEffectExecutionCalculation::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "Execute", &UGameplayEffectExecutionCalculation::execExecute },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics
	{
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_OutExecutionOutput;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ExecutionParams_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_ExecutionParams;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::NewProp_OutExecutionOutput = { "OutExecutionOutput", nullptr, (EPropertyFlags)0x0010000000000180, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayEffectExecutionCalculation_eventExecute_Parms, OutExecutionOutput), Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionOutput, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::NewProp_ExecutionParams_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::NewProp_ExecutionParams = { "ExecutionParams", nullptr, (EPropertyFlags)0x0010000008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayEffectExecutionCalculation_eventExecute_Parms, ExecutionParams), Z_Construct_UScriptStruct_FGameplayEffectCustomExecutionParameters, METADATA_PARAMS(Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::NewProp_ExecutionParams_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::NewProp_ExecutionParams_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::NewProp_OutExecutionOutput,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::NewProp_ExecutionParams,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::Function_MetaDataParams[] = {
		{ "Category", "Calculation" },
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "Called whenever the owning gameplay effect is executed. Allowed to do essentially whatever is desired, including generating new\nmodifiers to instantly execute as well.\n\n@note: Native subclasses should override the auto-generated Execute_Implementation function and NOT this one.\n\n@param ExecutionParams               Parameters for the custom execution calculation\n@param OutExecutionOutput    [OUT] Output data populated by the execution detailing further behavior or results of the execution" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayEffectExecutionCalculation, nullptr, "Execute", sizeof(GameplayEffectExecutionCalculation_eventExecute_Parms), Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x48420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UGameplayEffectExecutionCalculation_NoRegister()
	{
		return UGameplayEffectExecutionCalculation::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_EDITORONLY_DATA
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_InvalidScopedModifierAttributes_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_InvalidScopedModifierAttributes;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_InvalidScopedModifierAttributes_Inner;
#endif // WITH_EDITORONLY_DATA
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bRequiresPassedInTags_MetaData[];
#endif
		static void NewProp_bRequiresPassedInTags_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bRequiresPassedInTags;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_EDITORONLY_DATA
#endif // WITH_EDITORONLY_DATA
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayEffectCalculation,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayEffectExecutionCalculation_Execute, "Execute" }, // 4183443591
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayEffectExecutionCalculation.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
	};
#endif
#if WITH_EDITORONLY_DATA
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_InvalidScopedModifierAttributes_MetaData[] = {
		{ "Category", "Attributes" },
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "Any attribute in this list will not show up as a valid option for scoped modifiers; Used to allow attribute capture for internal calculation while preventing modification" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_InvalidScopedModifierAttributes = { "InvalidScopedModifierAttributes", nullptr, (EPropertyFlags)0x0020080800010001, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayEffectExecutionCalculation, InvalidScopedModifierAttributes), METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_InvalidScopedModifierAttributes_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_InvalidScopedModifierAttributes_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_InvalidScopedModifierAttributes_Inner = { "InvalidScopedModifierAttributes", nullptr, (EPropertyFlags)0x0000000800000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayEffectAttributeCaptureDefinition, METADATA_PARAMS(nullptr, 0) };
#endif // WITH_EDITORONLY_DATA
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_bRequiresPassedInTags_MetaData[] = {
		{ "Category", "Attributes" },
		{ "ModuleRelativePath", "Public/GameplayEffectExecutionCalculation.h" },
		{ "ToolTip", "Used to indicate if this execution uses Passed In Tags" },
	};
#endif
	void Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_bRequiresPassedInTags_SetBit(void* Obj)
	{
		((UGameplayEffectExecutionCalculation*)Obj)->bRequiresPassedInTags = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_bRequiresPassedInTags = { "bRequiresPassedInTags", nullptr, (EPropertyFlags)0x0020080000010015, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UGameplayEffectExecutionCalculation), &Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_bRequiresPassedInTags_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_bRequiresPassedInTags_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_bRequiresPassedInTags_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::PropPointers[] = {
#if WITH_EDITORONLY_DATA
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_InvalidScopedModifierAttributes,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_InvalidScopedModifierAttributes_Inner,
#endif // WITH_EDITORONLY_DATA
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::NewProp_bRequiresPassedInTags,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayEffectExecutionCalculation>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::ClassParams = {
		&UGameplayEffectExecutionCalculation::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::PropPointers),
		0,
		0x001000A1u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayEffectExecutionCalculation()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayEffectExecutionCalculation_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayEffectExecutionCalculation, 779539841);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayEffectExecutionCalculation>()
	{
		return UGameplayEffectExecutionCalculation::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayEffectExecutionCalculation(Z_Construct_UClass_UGameplayEffectExecutionCalculation, &UGameplayEffectExecutionCalculation::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayEffectExecutionCalculation"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayEffectExecutionCalculation);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
