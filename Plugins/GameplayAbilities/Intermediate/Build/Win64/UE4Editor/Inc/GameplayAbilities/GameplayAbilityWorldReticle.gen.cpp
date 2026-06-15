// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityWorldReticle.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilityWorldReticle() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FWorldReticleParameters();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector();
	ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_APlayerController_NoRegister();
// End Cross Module References
class UScriptStruct* FWorldReticleParameters::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FWorldReticleParameters_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FWorldReticleParameters, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("WorldReticleParameters"), sizeof(FWorldReticleParameters), Get_Z_Construct_UScriptStruct_FWorldReticleParameters_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FWorldReticleParameters>()
{
	return FWorldReticleParameters::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FWorldReticleParameters(FWorldReticleParameters::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("WorldReticleParameters"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFWorldReticleParameters
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFWorldReticleParameters()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("WorldReticleParameters")),new UScriptStruct::TCppStructOps<FWorldReticleParameters>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFWorldReticleParameters;
	struct Z_Construct_UScriptStruct_FWorldReticleParameters_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AOEScale_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_AOEScale;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FWorldReticleParameters>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::NewProp_AOEScale_MetaData[] = {
		{ "Category", "Reticle" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::NewProp_AOEScale = { "AOEScale", nullptr, (EPropertyFlags)0x0010000000000004, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FWorldReticleParameters, AOEScale), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::NewProp_AOEScale_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::NewProp_AOEScale_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::NewProp_AOEScale,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"WorldReticleParameters",
		sizeof(FWorldReticleParameters),
		alignof(FWorldReticleParameters),
		Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FWorldReticleParameters()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FWorldReticleParameters_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("WorldReticleParameters"), sizeof(FWorldReticleParameters), Get_Z_Construct_UScriptStruct_FWorldReticleParameters_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FWorldReticleParameters_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FWorldReticleParameters_Hash() { return 1053090733U; }
	static FName NAME_AGameplayAbilityWorldReticle_OnParametersInitialized = FName(TEXT("OnParametersInitialized"));
	void AGameplayAbilityWorldReticle::OnParametersInitialized()
	{
		ProcessEvent(FindFunctionChecked(NAME_AGameplayAbilityWorldReticle_OnParametersInitialized),NULL);
	}
	static FName NAME_AGameplayAbilityWorldReticle_OnTargetingAnActor = FName(TEXT("OnTargetingAnActor"));
	void AGameplayAbilityWorldReticle::OnTargetingAnActor(bool bNewValue)
	{
		GameplayAbilityWorldReticle_eventOnTargetingAnActor_Parms Parms;
		Parms.bNewValue=bNewValue ? true : false;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayAbilityWorldReticle_OnTargetingAnActor),&Parms);
	}
	static FName NAME_AGameplayAbilityWorldReticle_OnValidTargetChanged = FName(TEXT("OnValidTargetChanged"));
	void AGameplayAbilityWorldReticle::OnValidTargetChanged(bool bNewValue)
	{
		GameplayAbilityWorldReticle_eventOnValidTargetChanged_Parms Parms;
		Parms.bNewValue=bNewValue ? true : false;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayAbilityWorldReticle_OnValidTargetChanged),&Parms);
	}
	static FName NAME_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat = FName(TEXT("SetReticleMaterialParamFloat"));
	void AGameplayAbilityWorldReticle::SetReticleMaterialParamFloat(FName ParamName, float value)
	{
		GameplayAbilityWorldReticle_eventSetReticleMaterialParamFloat_Parms Parms;
		Parms.ParamName=ParamName;
		Parms.value=value;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat),&Parms);
	}
	static FName NAME_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector = FName(TEXT("SetReticleMaterialParamVector"));
	void AGameplayAbilityWorldReticle::SetReticleMaterialParamVector(FName ParamName, FVector value)
	{
		GameplayAbilityWorldReticle_eventSetReticleMaterialParamVector_Parms Parms;
		Parms.ParamName=ParamName;
		Parms.value=value;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector),&Parms);
	}
	void AGameplayAbilityWorldReticle::StaticRegisterNativesAGameplayAbilityWorldReticle()
	{
		UClass* Class = AGameplayAbilityWorldReticle::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "FaceTowardSource", &AGameplayAbilityWorldReticle::execFaceTowardSource },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics
	{
		struct GameplayAbilityWorldReticle_eventFaceTowardSource_Parms
		{
			bool bFaceIn2D;
		};
		static void NewProp_bFaceIn2D_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bFaceIn2D;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::NewProp_bFaceIn2D_SetBit(void* Obj)
	{
		((GameplayAbilityWorldReticle_eventFaceTowardSource_Parms*)Obj)->bFaceIn2D = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::NewProp_bFaceIn2D = { "bFaceIn2D", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayAbilityWorldReticle_eventFaceTowardSource_Parms), &Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::NewProp_bFaceIn2D_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::NewProp_bFaceIn2D,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::Function_MetaDataParams[] = {
		{ "Category", "Reticle" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayAbilityWorldReticle, nullptr, "FaceTowardSource", sizeof(GameplayAbilityWorldReticle_eventFaceTowardSource_Parms), Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized_Statics::Function_MetaDataParams[] = {
		{ "Category", "Reticle" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayAbilityWorldReticle, nullptr, "OnParametersInitialized", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics
	{
		static void NewProp_bNewValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bNewValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::NewProp_bNewValue_SetBit(void* Obj)
	{
		((GameplayAbilityWorldReticle_eventOnTargetingAnActor_Parms*)Obj)->bNewValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::NewProp_bNewValue = { "bNewValue", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayAbilityWorldReticle_eventOnTargetingAnActor_Parms), &Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::NewProp_bNewValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::NewProp_bNewValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::Function_MetaDataParams[] = {
		{ "Category", "Reticle" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "Called whenever bIsTargetAnActor changes value." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayAbilityWorldReticle, nullptr, "OnTargetingAnActor", sizeof(GameplayAbilityWorldReticle_eventOnTargetingAnActor_Parms), Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics
	{
		static void NewProp_bNewValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bNewValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::NewProp_bNewValue_SetBit(void* Obj)
	{
		((GameplayAbilityWorldReticle_eventOnValidTargetChanged_Parms*)Obj)->bNewValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::NewProp_bNewValue = { "bNewValue", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayAbilityWorldReticle_eventOnValidTargetChanged_Parms), &Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::NewProp_bNewValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::NewProp_bNewValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::Function_MetaDataParams[] = {
		{ "Category", "Reticle" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "Called whenever bIsTargetValid changes value." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayAbilityWorldReticle, nullptr, "OnValidTargetChanged", sizeof(GameplayAbilityWorldReticle_eventOnValidTargetChanged_Parms), Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics
	{
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_value;
		static const UE4CodeGen_Private::FNamePropertyParams NewProp_ParamName;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::NewProp_value = { "value", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayAbilityWorldReticle_eventSetReticleMaterialParamFloat_Parms, value), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::NewProp_ParamName = { "ParamName", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayAbilityWorldReticle_eventSetReticleMaterialParamFloat_Parms, ParamName), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::NewProp_value,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::NewProp_ParamName,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::Function_MetaDataParams[] = {
		{ "Category", "Reticle" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayAbilityWorldReticle, nullptr, "SetReticleMaterialParamFloat", sizeof(GameplayAbilityWorldReticle_eventSetReticleMaterialParamFloat_Parms), Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics
	{
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_value;
		static const UE4CodeGen_Private::FNamePropertyParams NewProp_ParamName;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::NewProp_value = { "value", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayAbilityWorldReticle_eventSetReticleMaterialParamVector_Parms, value), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::NewProp_ParamName = { "ParamName", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayAbilityWorldReticle_eventSetReticleMaterialParamVector_Parms, ParamName), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::NewProp_value,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::NewProp_ParamName,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::Function_MetaDataParams[] = {
		{ "Category", "Reticle" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayAbilityWorldReticle, nullptr, "SetReticleMaterialParamVector", sizeof(GameplayAbilityWorldReticle_eventSetReticleMaterialParamVector_Parms), Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08820800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle_NoRegister()
	{
		return AGameplayAbilityWorldReticle::StaticClass();
	}
	struct Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_TargetingActor_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_TargetingActor;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MasterPC_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MasterPC;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bIsTargetAnActor_MetaData[];
#endif
		static void NewProp_bIsTargetAnActor_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bIsTargetAnActor;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bIsTargetValid_MetaData[];
#endif
		static void NewProp_bIsTargetValid_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bIsTargetValid;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bSnapToTargetedActor_MetaData[];
#endif
		static void NewProp_bSnapToTargetedActor_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bSnapToTargetedActor;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bFaceOwnerFlat_MetaData[];
#endif
		static void NewProp_bFaceOwnerFlat_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bFaceOwnerFlat;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Parameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_AGameplayAbilityWorldReticle_FaceTowardSource, "FaceTowardSource" }, // 3721535385
		{ &Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnParametersInitialized, "OnParametersInitialized" }, // 2012413810
		{ &Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnTargetingAnActor, "OnTargetingAnActor" }, // 2224634082
		{ &Z_Construct_UFunction_AGameplayAbilityWorldReticle_OnValidTargetChanged, "OnValidTargetChanged" }, // 766928664
		{ &Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamFloat, "SetReticleMaterialParamFloat" }, // 3967610870
		{ &Z_Construct_UFunction_AGameplayAbilityWorldReticle_SetReticleMaterialParamVector, "SetReticleMaterialParamVector" }, // 3574405522
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "Abilities/GameplayAbilityWorldReticle.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "Reticles allow targeting to be visualized. Tasks can spawn these. Artists/designers can create BPs for these." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_TargetingActor_MetaData[] = {
		{ "Category", "Network" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "In the future, we may want to grab things like sockets off of this." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_TargetingActor = { "TargetingActor", nullptr, (EPropertyFlags)0x0020080000000014, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayAbilityWorldReticle, TargetingActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_TargetingActor_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_TargetingActor_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_MasterPC_MetaData[] = {
		{ "Category", "Network" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "This is used in the process of determining whether we should replicate to a specific client." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_MasterPC = { "MasterPC", nullptr, (EPropertyFlags)0x0020080000000014, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayAbilityWorldReticle, MasterPC), Z_Construct_UClass_APlayerController_NoRegister, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_MasterPC_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_MasterPC_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetAnActor_MetaData[] = {
		{ "Category", "Network" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "This indicates whether or not the targeting reticle is pointed at an actor. Defaults to false." },
	};
#endif
	void Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetAnActor_SetBit(void* Obj)
	{
		((AGameplayAbilityWorldReticle*)Obj)->bIsTargetAnActor = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetAnActor = { "bIsTargetAnActor", nullptr, (EPropertyFlags)0x0020080000000014, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayAbilityWorldReticle), &Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetAnActor_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetAnActor_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetAnActor_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetValid_MetaData[] = {
		{ "Category", "Network" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "This indicates whether or not the targeting actor considers the current target to be valid. Defaults to true." },
	};
#endif
	void Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetValid_SetBit(void* Obj)
	{
		((AGameplayAbilityWorldReticle*)Obj)->bIsTargetValid = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetValid = { "bIsTargetValid", nullptr, (EPropertyFlags)0x0020080000000014, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayAbilityWorldReticle), &Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetValid_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetValid_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetValid_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bSnapToTargetedActor_MetaData[] = {
		{ "Category", "Reticle" },
		{ "ExposeOnSpawn", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "If the target is an actor snap to it's location" },
	};
#endif
	void Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bSnapToTargetedActor_SetBit(void* Obj)
	{
		((AGameplayAbilityWorldReticle*)Obj)->bSnapToTargetedActor = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bSnapToTargetedActor = { "bSnapToTargetedActor", nullptr, (EPropertyFlags)0x0011000000000005, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayAbilityWorldReticle), &Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bSnapToTargetedActor_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bSnapToTargetedActor_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bSnapToTargetedActor_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bFaceOwnerFlat_MetaData[] = {
		{ "Category", "Reticle" },
		{ "ExposeOnSpawn", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
		{ "ToolTip", "Makes the reticle's default owner-facing behavior operate in 2D (flat) instead of 3D (pitched). Defaults to true." },
	};
#endif
	void Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bFaceOwnerFlat_SetBit(void* Obj)
	{
		((AGameplayAbilityWorldReticle*)Obj)->bFaceOwnerFlat = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bFaceOwnerFlat = { "bFaceOwnerFlat", nullptr, (EPropertyFlags)0x0011000000000005, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayAbilityWorldReticle), &Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bFaceOwnerFlat_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bFaceOwnerFlat_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bFaceOwnerFlat_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_Parameters_MetaData[] = {
		{ "Category", "Reticle" },
		{ "ExposeOnSpawn", "true" },
		{ "ModuleRelativePath", "Public/Abilities/GameplayAbilityWorldReticle.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0011000000000805, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayAbilityWorldReticle, Parameters), Z_Construct_UScriptStruct_FWorldReticleParameters, METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_TargetingActor,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_MasterPC,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetAnActor,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bIsTargetValid,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bSnapToTargetedActor,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_bFaceOwnerFlat,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::NewProp_Parameters,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameplayAbilityWorldReticle>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::ClassParams = {
		&AGameplayAbilityWorldReticle::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::PropPointers),
		0,
		0x009002A0u,
		METADATA_PARAMS(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGameplayAbilityWorldReticle()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGameplayAbilityWorldReticle_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGameplayAbilityWorldReticle, 1299001012);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<AGameplayAbilityWorldReticle>()
	{
		return AGameplayAbilityWorldReticle::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGameplayAbilityWorldReticle(Z_Construct_UClass_AGameplayAbilityWorldReticle, &AGameplayAbilityWorldReticle::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("AGameplayAbilityWorldReticle"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGameplayAbilityWorldReticle);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
