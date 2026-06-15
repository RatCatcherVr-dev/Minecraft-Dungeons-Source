// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCueNotify_Actor.h"
#include "Serialization/ArchiveUObjectFromStructuredArchive.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCueNotify_Actor() {}
// Cross Module References
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayCueNotify_Actor_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayCueNotify_Actor();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueParameters();
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayCueEvent();
	ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTagReferenceHelper();
	GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
// End Cross Module References
	static FName NAME_AGameplayCueNotify_Actor_K2_HandleGameplayCue = FName(TEXT("K2_HandleGameplayCue"));
	void AGameplayCueNotify_Actor::K2_HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, FGameplayCueParameters const& Parameters)
	{
		GameplayCueNotify_Actor_eventK2_HandleGameplayCue_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.EventType=EventType;
		Parms.Parameters=Parameters;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayCueNotify_Actor_K2_HandleGameplayCue),&Parms);
	}
	static FName NAME_AGameplayCueNotify_Actor_OnActive = FName(TEXT("OnActive"));
	bool AGameplayCueNotify_Actor::OnActive(AActor* MyTarget, FGameplayCueParameters const& Parameters)
	{
		GameplayCueNotify_Actor_eventOnActive_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayCueNotify_Actor_OnActive),&Parms);
		return !!Parms.ReturnValue;
	}
	static FName NAME_AGameplayCueNotify_Actor_OnExecute = FName(TEXT("OnExecute"));
	bool AGameplayCueNotify_Actor::OnExecute(AActor* MyTarget, FGameplayCueParameters const& Parameters)
	{
		GameplayCueNotify_Actor_eventOnExecute_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayCueNotify_Actor_OnExecute),&Parms);
		return !!Parms.ReturnValue;
	}
	static FName NAME_AGameplayCueNotify_Actor_OnRemove = FName(TEXT("OnRemove"));
	bool AGameplayCueNotify_Actor::OnRemove(AActor* MyTarget, FGameplayCueParameters const& Parameters)
	{
		GameplayCueNotify_Actor_eventOnRemove_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayCueNotify_Actor_OnRemove),&Parms);
		return !!Parms.ReturnValue;
	}
	static FName NAME_AGameplayCueNotify_Actor_WhileActive = FName(TEXT("WhileActive"));
	bool AGameplayCueNotify_Actor::WhileActive(AActor* MyTarget, FGameplayCueParameters const& Parameters)
	{
		GameplayCueNotify_Actor_eventWhileActive_Parms Parms;
		Parms.MyTarget=MyTarget;
		Parms.Parameters=Parameters;
		ProcessEvent(FindFunctionChecked(NAME_AGameplayCueNotify_Actor_WhileActive),&Parms);
		return !!Parms.ReturnValue;
	}
	void AGameplayCueNotify_Actor::StaticRegisterNativesAGameplayCueNotify_Actor()
	{
		UClass* Class = AGameplayCueNotify_Actor::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "K2_EndGameplayCue", &AGameplayCueNotify_Actor::execK2_EndGameplayCue },
			{ "OnActive", &AGameplayCueNotify_Actor::execOnActive },
			{ "OnExecute", &AGameplayCueNotify_Actor::execOnExecute },
			{ "OnOwnerDestroyed", &AGameplayCueNotify_Actor::execOnOwnerDestroyed },
			{ "OnRemove", &AGameplayCueNotify_Actor::execOnRemove },
			{ "WhileActive", &AGameplayCueNotify_Actor::execWhileActive },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "DisplayName", "End (Recycle) GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ScriptName", "EndGameplayCue" },
		{ "ToolTip", "Ends the gameplay cue: either destroying it or recycling it. You must call this manually only if you do not use bAutoDestroyOnRemove/AutoDestroyDelay" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCueNotify_Actor, nullptr, "K2_EndGameplayCue", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020400, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics
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
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventK2_HandleGameplayCue_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_EventType = { "EventType", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventK2_HandleGameplayCue_Parms, EventType), Z_Construct_UEnum_GameplayAbilities_EGameplayCueEvent, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventK2_HandleGameplayCue_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_EventType,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "DisplayName", "HandleGameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ScriptName", "HandleGameplayCue" },
		{ "ToolTip", "Generic Event Graph event that will get called for every event type" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCueNotify_Actor, nullptr, "K2_HandleGameplayCue", sizeof(GameplayCueNotify_Actor_eventK2_HandleGameplayCue_Parms), Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08420800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics
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
	void Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Actor_eventOnActive_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Actor_eventOnActive_Parms), &Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventOnActive_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventOnActive_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCueNotify_Actor, nullptr, "OnActive", sizeof(GameplayCueNotify_Actor_eventOnActive_Parms), Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics
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
	void Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Actor_eventOnExecute_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Actor_eventOnExecute_Parms), &Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventOnExecute_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventOnExecute_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCueNotify_Actor, nullptr, "OnExecute", sizeof(GameplayCueNotify_Actor_eventOnExecute_Parms), Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics
	{
		struct GameplayCueNotify_Actor_eventOnOwnerDestroyed_Parms
		{
			AActor* DestroyedActor;
		};
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_DestroyedActor;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::NewProp_DestroyedActor = { "DestroyedActor", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventOnOwnerDestroyed_Parms, DestroyedActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::NewProp_DestroyedActor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCueNotify_Actor, nullptr, "OnOwnerDestroyed", sizeof(GameplayCueNotify_Actor_eventOnOwnerDestroyed_Parms), Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020400, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics
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
	void Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Actor_eventOnRemove_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Actor_eventOnRemove_Parms), &Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventOnRemove_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventOnRemove_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCueNotify_Actor, nullptr, "OnRemove", sizeof(GameplayCueNotify_Actor_eventOnRemove_Parms), Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics
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
	void Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((GameplayCueNotify_Actor_eventWhileActive_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(GameplayCueNotify_Actor_eventWhileActive_Parms), &Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_Parameters_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_Parameters = { "Parameters", nullptr, (EPropertyFlags)0x0010008008000182, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventWhileActive_Parms, Parameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_Parameters_MetaData, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_Parameters_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_MyTarget = { "MyTarget", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(GameplayCueNotify_Actor_eventWhileActive_Parms, MyTarget), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_Parameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::NewProp_MyTarget,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::Function_MetaDataParams[] = {
		{ "Category", "GameplayCueNotify" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AGameplayCueNotify_Actor, nullptr, "WhileActive", sizeof(GameplayCueNotify_Actor_eventWhileActive_Parms), Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08420C00, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_AGameplayCueNotify_Actor_NoRegister()
	{
		return AGameplayCueNotify_Actor::StaticClass();
	}
	struct Z_Construct_UClass_AGameplayCueNotify_Actor_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NumPreallocatedInstances_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_NumPreallocatedInstances;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bAllowMultipleWhileActiveEvents_MetaData[];
#endif
		static void NewProp_bAllowMultipleWhileActiveEvents_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bAllowMultipleWhileActiveEvents;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bAllowMultipleOnActiveEvents_MetaData[];
#endif
		static void NewProp_bAllowMultipleOnActiveEvents_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bAllowMultipleOnActiveEvents;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bUniqueInstancePerSourceObject_MetaData[];
#endif
		static void NewProp_bUniqueInstancePerSourceObject_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bUniqueInstancePerSourceObject;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bUniqueInstancePerInstigator_MetaData[];
#endif
		static void NewProp_bUniqueInstancePerInstigator_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bUniqueInstancePerInstigator;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_IsOverride_MetaData[];
#endif
		static void NewProp_IsOverride_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_IsOverride;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bAutoAttachToOwner_MetaData[];
#endif
		static void NewProp_bAutoAttachToOwner_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bAutoAttachToOwner;
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
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_WarnIfLatentActionIsStillRunning_MetaData[];
#endif
		static void NewProp_WarnIfLatentActionIsStillRunning_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_WarnIfLatentActionIsStillRunning;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_WarnIfTimelineIsStillRunning_MetaData[];
#endif
		static void NewProp_WarnIfTimelineIsStillRunning_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_WarnIfTimelineIsStillRunning;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AutoDestroyDelay_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_AutoDestroyDelay;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bAutoDestroyOnRemove_MetaData[];
#endif
		static void NewProp_bAutoDestroyOnRemove_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bAutoDestroyOnRemove;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_EndGameplayCue, "K2_EndGameplayCue" }, // 2763616139
		{ &Z_Construct_UFunction_AGameplayCueNotify_Actor_K2_HandleGameplayCue, "K2_HandleGameplayCue" }, // 2506891634
		{ &Z_Construct_UFunction_AGameplayCueNotify_Actor_OnActive, "OnActive" }, // 3245504485
		{ &Z_Construct_UFunction_AGameplayCueNotify_Actor_OnExecute, "OnExecute" }, // 4101679227
		{ &Z_Construct_UFunction_AGameplayCueNotify_Actor_OnOwnerDestroyed, "OnOwnerDestroyed" }, // 1349056034
		{ &Z_Construct_UFunction_AGameplayCueNotify_Actor_OnRemove, "OnRemove" }, // 2155763284
		{ &Z_Construct_UFunction_AGameplayCueNotify_Actor_WhileActive, "WhileActive" }, // 971547622
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "HideCategories", "Replication" },
		{ "IncludePath", "GameplayCueNotify_Actor.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "SerializeToFArchive", "" },
		{ "ShowWorldContextPin", "" },
		{ "ToolTip", "An instantiated Actor that acts as a handler of a GameplayCue. Since they are instantiated, they can maintain state and tick/update every frame if necessary." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_NumPreallocatedInstances_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "How many instances of the gameplay cue to preallocate" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_NumPreallocatedInstances = { "NumPreallocatedInstances", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayCueNotify_Actor, NumPreallocatedInstances), METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_NumPreallocatedInstances_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_NumPreallocatedInstances_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleWhileActiveEvents_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Does this cue trigger its WhileActive event if it's already been triggered?\nThis can occur when the associated tag is triggered by multiple sources and there is no unique instancing." },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleWhileActiveEvents_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->bAllowMultipleWhileActiveEvents = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleWhileActiveEvents = { "bAllowMultipleWhileActiveEvents", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleWhileActiveEvents_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleWhileActiveEvents_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleWhileActiveEvents_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleOnActiveEvents_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Does this cue trigger its OnActive event if it's already been triggered?\nThis can occur when the associated tag is triggered by multiple sources and there is no unique instancing." },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleOnActiveEvents_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->bAllowMultipleOnActiveEvents = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleOnActiveEvents = { "bAllowMultipleOnActiveEvents", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleOnActiveEvents_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleOnActiveEvents_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleOnActiveEvents_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerSourceObject_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Does this cue get a new instance for each source object? For example if two source objects apply a GC to the same source, do we create two of these GameplayCue Notify actors or just one?\nIf the notify is simply playing FX or sounds on the source, it should not need unique instances. If this Notify is attaching a beam from the source object to the target, it does need a unique instance per instigator." },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerSourceObject_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->bUniqueInstancePerSourceObject = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerSourceObject = { "bUniqueInstancePerSourceObject", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerSourceObject_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerSourceObject_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerSourceObject_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerInstigator_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Does this cue get a new instance for each instigator? For example if two instigators apply a GC to the same source, do we create two of these GameplayCue Notify actors or just one?\nIf the notify is simply playing FX or sounds on the source, it should not need unique instances. If this Notify is attaching a beam from the instigator to the target, it does need a unique instance per instigator." },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerInstigator_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->bUniqueInstancePerInstigator = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerInstigator = { "bUniqueInstancePerInstigator", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerInstigator_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerInstigator_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerInstigator_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_IsOverride_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Does this Cue override other cues, or is it called in addition to them? E.g., If this is Damage.Physical.Slash, we wont call Damage.Physical afer we run this cue." },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_IsOverride_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->IsOverride = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_IsOverride = { "IsOverride", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_IsOverride_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_IsOverride_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_IsOverride_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoAttachToOwner_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "If true, attach this GameplayCue Actor to the target actor while it is active. Attaching is slightly more expensive than not attaching, so only enable when you need to." },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoAttachToOwner_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->bAutoAttachToOwner = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoAttachToOwner = { "bAutoAttachToOwner", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoAttachToOwner_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoAttachToOwner_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoAttachToOwner_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueName_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Mirrors GameplayCueTag in order to be asset registry searchable" },
	};
#endif
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueName = { "GameplayCueName", nullptr, (EPropertyFlags)0x0010010000000000, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayCueNotify_Actor, GameplayCueName), METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueName_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueName_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_ReferenceHelper_MetaData[] = {
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_ReferenceHelper = { "ReferenceHelper", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayCueNotify_Actor, ReferenceHelper), Z_Construct_UScriptStruct_FGameplayTagReferenceHelper, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_ReferenceHelper_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_ReferenceHelper_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueTag_MetaData[] = {
		{ "Categories", "GameplayCue" },
		{ "Category", "GameplayCue" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueTag = { "GameplayCueTag", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayCueNotify_Actor, GameplayCueTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueTag_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueTag_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfLatentActionIsStillRunning_MetaData[] = {
		{ "Category", "Cleanup" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Warn if we have a latent action (delay, etc) running when we cleanup this gameplay cue (we will kill the latent action either way)" },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfLatentActionIsStillRunning_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->WarnIfLatentActionIsStillRunning = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfLatentActionIsStillRunning = { "WarnIfLatentActionIsStillRunning", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfLatentActionIsStillRunning_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfLatentActionIsStillRunning_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfLatentActionIsStillRunning_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfTimelineIsStillRunning_MetaData[] = {
		{ "Category", "Cleanup" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "Warn if we have a timeline running when we cleanup this gameplay cue (we will kill the timeline either way)" },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfTimelineIsStillRunning_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->WarnIfTimelineIsStillRunning = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfTimelineIsStillRunning = { "WarnIfTimelineIsStillRunning", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfTimelineIsStillRunning_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfTimelineIsStillRunning_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfTimelineIsStillRunning_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_AutoDestroyDelay_MetaData[] = {
		{ "Category", "Cleanup" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "If bAutoDestroyOnRemove is true, the actor will stay alive for this many seconds before being auto destroyed." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_AutoDestroyDelay = { "AutoDestroyDelay", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AGameplayCueNotify_Actor, AutoDestroyDelay), METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_AutoDestroyDelay_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_AutoDestroyDelay_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoDestroyOnRemove_MetaData[] = {
		{ "Category", "Cleanup" },
		{ "ModuleRelativePath", "Public/GameplayCueNotify_Actor.h" },
		{ "ToolTip", "We will auto destroy (recycle) this GameplayCueActor when the OnRemove event fires (after OnRemove is called)." },
	};
#endif
	void Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoDestroyOnRemove_SetBit(void* Obj)
	{
		((AGameplayCueNotify_Actor*)Obj)->bAutoDestroyOnRemove = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoDestroyOnRemove = { "bAutoDestroyOnRemove", nullptr, (EPropertyFlags)0x0010000000010001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AGameplayCueNotify_Actor), &Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoDestroyOnRemove_SetBit, METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoDestroyOnRemove_MetaData, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoDestroyOnRemove_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_NumPreallocatedInstances,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleWhileActiveEvents,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAllowMultipleOnActiveEvents,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerSourceObject,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bUniqueInstancePerInstigator,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_IsOverride,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoAttachToOwner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueName,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_ReferenceHelper,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_GameplayCueTag,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfLatentActionIsStillRunning,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_WarnIfTimelineIsStillRunning,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_AutoDestroyDelay,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::NewProp_bAutoDestroyOnRemove,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGameplayCueNotify_Actor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::ClassParams = {
		&AGameplayCueNotify_Actor::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::PropPointers),
		0,
		0x009000A0u,
		METADATA_PARAMS(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGameplayCueNotify_Actor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGameplayCueNotify_Actor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGameplayCueNotify_Actor, 848588191);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<AGameplayCueNotify_Actor>()
	{
		return AGameplayCueNotify_Actor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGameplayCueNotify_Actor(Z_Construct_UClass_AGameplayCueNotify_Actor, &AGameplayCueNotify_Actor::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("AGameplayCueNotify_Actor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGameplayCueNotify_Actor);
	IMPLEMENT_FSTRUCTUREDARCHIVE_SERIALIZER(AGameplayCueNotify_Actor)
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
