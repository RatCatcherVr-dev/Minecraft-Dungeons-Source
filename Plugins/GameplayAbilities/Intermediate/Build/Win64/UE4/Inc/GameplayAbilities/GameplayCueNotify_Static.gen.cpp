// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCueNotify_Static.h"
#include "Serialization/ArchiveUObjectFromStructuredArchive.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCueNotify_Static() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueNotify_Static_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayCueNotify_Static();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueParameters();
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayCueEvent();
	ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTagReferenceHelper();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
// End Cross Module References
	static FName NAME_UGameplayCueNotify_Static_K2_HandleGameplayCue = FName(TEXT("K2_HandleGameplayCue"));
	void UGameplayCueNotify_Static::K2_HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, FGameplayCueParameters const& Parameters) const
	{
		GameplayCueNotify_Static_eventK2_HandleGameplayCue_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.EventType=EventType;
		Parms.Parameters=Parameters;
		const_cast<UGameplayCueNotify_Static*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayCueNotify_Static_K2_HandleGameplayCue),&Parms);
	}
	static FName NAME_UGameplayCueNotify_Static_OnActive = FName(TEXT("OnActive"));
	bool UGameplayCueNotify_Static::OnActive(AActor* MyTarget, FGameplayCueParameters const& Parameters) const
	{
		GameplayCueNotify_Static_eventOnActive_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		const_cast<UGameplayCueNotify_Static*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayCueNotify_Static_OnActive),&Parms);
		return !!Parms.ReturnValue;
	}
	static FName NAME_UGameplayCueNotify_Static_OnExecute = FName(TEXT("OnExecute"));
	bool UGameplayCueNotify_Static::OnExecute(AActor* MyTarget, FGameplayCueParameters const& Parameters) const
	{
		GameplayCueNotify_Static_eventOnExecute_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		const_cast<UGameplayCueNotify_Static*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayCueNotify_Static_OnExecute),&Parms);
		return !!Parms.ReturnValue;
	}
	static FName NAME_UGameplayCueNotify_Static_OnRemove = FName(TEXT("OnRemove"));
	bool UGameplayCueNotify_Static::OnRemove(AActor* MyTarget, FGameplayCueParameters const& Parameters) const
	{
		GameplayCueNotify_Static_eventOnRemove_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		const_cast<UGameplayCueNotify_Static*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayCueNotify_Static_OnRemove),&Parms);
		return !!Parms.ReturnValue;
	}
	static FName NAME_UGameplayCueNotify_Static_WhileActive = FName(TEXT("WhileActive"));
	bool UGameplayCueNotify_Static::WhileActive(AActor* MyTarget, FGameplayCueParameters const& Parameters) const
	{
		GameplayCueNotify_Static_eventWhileActive_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		const_cast<UGameplayCueNotify_Static*>(this)->ProcessEvent(FindFunctionChecked(NAME_UGameplayCueNotify_Static_WhileActive),&Parms);
		return !!Parms.ReturnValue;
	}
	void UGameplayCueNotify_Static::StaticRegisterNativesUGameplayCueNotify_Static()
	{
		UClass* Class = UGameplayCueNotify_Static::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "OnActive", &UGameplayCueNotify_Static::execOnActive },
			{ "OnExecute", &UGameplayCueNotify_Static::execOnExecute },
			{ "OnRemove", &UGameplayCueNotify_Static::execOnRemove },
			{ "WhileActive", &UGameplayCueNotify_Static::execWhileActive },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Parameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_EventType;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MyTarget;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventK2_HandleGameplayCue_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_EventType = { "EventType", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventK2_HandleGameplayCue_Parms, EventType), Z_Construct_UEnum_GameplayAbilities_EGameplayCueEvent, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventK2_HandleGameplayCue_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_EventType,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "DisplayName", "HandleGameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
		{ "ScriptName", "HandleGameplayCue" },
		{ "ToolTip", "Generic Event Graph event that will get called for every event type" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCueNotify_Static, nullptr, "K2_HandleGameplayCue", sizeof(GameplayCueNotify_Static_eventK2_HandleGameplayCue_Parms), Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x48420800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics
	{
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Parameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MyTarget;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Static_eventOnActive_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Static_eventOnActive_Parms), &Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventOnActive_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventOnActive_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCueNotify_Static, nullptr, "OnActive", sizeof(GameplayCueNotify_Static_eventOnActive_Parms), Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x5C420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics
	{
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Parameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MyTarget;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Static_eventOnExecute_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Static_eventOnExecute_Parms), &Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventOnExecute_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventOnExecute_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCueNotify_Static, nullptr, "OnExecute", sizeof(GameplayCueNotify_Static_eventOnExecute_Parms), Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x5C420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics
	{
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Parameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MyTarget;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Static_eventOnRemove_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Static_eventOnRemove_Parms), &Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventOnRemove_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventOnRemove_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCueNotify_Static, nullptr, "OnRemove", sizeof(GameplayCueNotify_Static_eventOnRemove_Parms), Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x5C420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics
	{
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Parameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Parameters;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MyTarget;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Static_eventWhileActive_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Static_eventWhileActive_Parms), &Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventWhileActive_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Static_eventWhileActive_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGameplayCueNotify_Static, nullptr, "WhileActive", sizeof(GameplayCueNotify_Static_eventWhileActive_Parms), Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x5C420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UGameplayCueNotify_Static_NoRegister()
	{
		return UGameplayCueNotify_Static::StaticClass();
	}
	struct Z_Construct_UClass_UGameplayCueNotify_Static_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_IsOverride_MetaData[];
#endif
		static void NewProp_IsOverride_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_IsOverride;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueName_MetaData[];
#endif
		static const UE4CodeGen_Private::FNamePropertyParams NewProp_GameplayCueName;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReferenceHelper_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_ReferenceHelper;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayCueTag_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayCueTag;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGameplayCueNotify_Static_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UGameplayCueNotify_Static_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UGameplayCueNotify_Static_K2_HandleGameplayCue, "K2_HandleGameplayCue" }, // 3562131237
		{ &Z_Construct_UFunction_UGameplayCueNotify_Static_OnActive, "OnActive" }, // 4049179189
		{ &Z_Construct_UFunction_UGameplayCueNotify_Static_OnExecute, "OnExecute" }, // 719523296
		{ &Z_Construct_UFunction_UGameplayCueNotify_Static_OnRemove, "OnRemove" }, // 975487707
		{ &Z_Construct_UFunction_UGameplayCueNotify_Static_WhileActive, "WhileActive" }, // 3580535142
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_Static_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "HideCategories", "Replication" },
		{ "IncludePath", "GameplayCueNotify_Static.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
		{ "SerializeToFArchive", "" },
		{ "ShowWorldContextPin", "" },
		{ "ToolTip", "A non instantiated UObject that acts as a handler for a GameplayCue. These are useful for one-off \"burst\" effects." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_IsOverride_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
		{ "ToolTip", "Does this Cue override other cues, or is it called in addition to them? E.g., If this is Damage.Physical.Slash, we wont call Damage.Physical afer we run this cue." },
	};
#endif
	void Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_IsOverride_SetBit(void* Obj)
	{
		((UGameplayCueNotify_Static*)Obj)->IsOverride = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_IsOverride = { "IsOverride", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UGameplayCueNotify_Static), &Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_IsOverride_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_IsOverride_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_IsOverride_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueName_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
		{ "ToolTip", "Mirrors GameplayCueTag in order to be asset registry searchable" },
	};
#endif
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueName = { "GameplayCueName", nullptr, (EPropertyFlags)0x0010010000000000, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueNotify_Static, GameplayCueName), METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueName_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueName_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_ReferenceHelper_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_ReferenceHelper = { "ReferenceHelper", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueNotify_Static, ReferenceHelper), Z_Construct_UScriptStruct_FGameplayTagReferenceHelper, METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_ReferenceHelper_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_ReferenceHelper_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueTag_MetaData[] = {
		{ "Categories", "GameplayCue" },
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Static.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueTag = { "GameplayCueTag", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UGameplayCueNotify_Static, GameplayCueTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueTag_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueTag_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGameplayCueNotify_Static_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_IsOverride,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueName,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_ReferenceHelper,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGameplayCueNotify_Static_Statics::NewProp_GameplayCueTag,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGameplayCueNotify_Static_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGameplayCueNotify_Static>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGameplayCueNotify_Static_Statics::ClassParams = {
		&UGameplayCueNotify_Static::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UGameplayCueNotify_Static_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGameplayCueNotify_Static_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGameplayCueNotify_Static()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGameplayCueNotify_Static_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGameplayCueNotify_Static, 1947002391);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UGameplayCueNotify_Static>()
	{
		return UGameplayCueNotify_Static::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGameplayCueNotify_Static(Z_Construct_UClass_UGameplayCueNotify_Static, &UGameplayCueNotify_Static::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UGameplayCueNotify_Static"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGameplayCueNotify_Static);
	IMPLEMENT_FSTRUCTUREDARCHIVE_SERIALIZER(UGameplayCueNotify_Static)
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
