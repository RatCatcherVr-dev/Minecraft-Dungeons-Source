// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/Tasks/AbilityTask_Repeat.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAbilityTask_Repeat() {}
// Cross Module References
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_Repeat_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_Repeat();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
// End Cross Module References
	struct Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics
	{
		struct _Script_GameplayAbilities_eventRepeatedActionDelegate_Parms
		{
			int32 ActionNumber;
		};
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_ActionNumber;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::NewProp_ActionNumber = { "ActionNumber", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(_Script_GameplayAbilities_eventRepeatedActionDelegate_Parms, ActionNumber), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::NewProp_ActionNumber,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_Repeat.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities, nullptr, "RepeatedActionDelegate__DelegateSignature", sizeof(_Script_GameplayAbilities_eventRepeatedActionDelegate_Parms), Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	void UAbilityTask_Repeat::StaticRegisterNativesUAbilityTask_Repeat()
	{
		UClass* Class = UAbilityTask_Repeat::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "RepeatAction", &UAbilityTask_Repeat::execRepeatAction },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics
	{
		struct AbilityTask_Repeat_eventRepeatAction_Parms
		{
			UGameplayAbility* OwningAbility;
			float TimeBetweenActions;
			int32 TotalActionCount;
			UAbilityTask_Repeat* ReturnValue;
		};
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_TotalActionCount;
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_TimeBetweenActions;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_OwningAbility;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_Repeat_eventRepeatAction_Parms, ReturnValue), Z_Construct_UClass_UAbilityTask_Repeat_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_TotalActionCount = { "TotalActionCount", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_Repeat_eventRepeatAction_Parms, TotalActionCount), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_TimeBetweenActions = { "TimeBetweenActions", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_Repeat_eventRepeatAction_Parms, TimeBetweenActions), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_OwningAbility = { "OwningAbility", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_Repeat_eventRepeatAction_Parms, OwningAbility), Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_TotalActionCount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_TimeBetweenActions,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::NewProp_OwningAbility,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::Function_MetaDataParams[] = {
		{ "BlueprintInternalUseOnly", "TRUE" },
		{ "Category", "Ability|Tasks" },
		{ "DefaultToSelf", "OwningAbility" },
		{ "HidePin", "OwningAbility" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_Repeat.h" },
		{ "ToolTip", "Start a task that repeats an action or set of actions." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAbilityTask_Repeat, nullptr, "RepeatAction", sizeof(AbilityTask_Repeat_eventRepeatAction_Parms), Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UAbilityTask_Repeat_NoRegister()
	{
		return UAbilityTask_Repeat::StaticClass();
	}
	struct Z_Construct_UClass_UAbilityTask_Repeat_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OnFinished_MetaData[];
#endif
		static const UE4CodeGen_Private::FMulticastDelegatePropertyParams NewProp_OnFinished;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OnPerformAction_MetaData[];
#endif
		static const UE4CodeGen_Private::FMulticastDelegatePropertyParams NewProp_OnPerformAction;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAbilityTask_Repeat_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UAbilityTask,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAbilityTask_Repeat_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAbilityTask_Repeat_RepeatAction, "RepeatAction" }, // 4175658144
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_Repeat_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/Tasks/AbilityTask_Repeat.h" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_Repeat.h" },
		{ "ToolTip", "Repeat a task a certain number of times at a given interval." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnFinished_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_Repeat.h" },
	};
#endif
	const UE4CodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnFinished = { "OnFinished", nullptr, (EPropertyFlags)0x0010000010080000, UE4CodeGen_Private::EPropertyGenFlags::MulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask_Repeat, OnFinished), Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnFinished_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnFinished_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnPerformAction_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_Repeat.h" },
	};
#endif
	const UE4CodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnPerformAction = { "OnPerformAction", nullptr, (EPropertyFlags)0x0010000010080000, UE4CodeGen_Private::EPropertyGenFlags::MulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask_Repeat, OnPerformAction), Z_Construct_UDelegateFunction_GameplayAbilities_RepeatedActionDelegate__DelegateSignature, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnPerformAction_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnPerformAction_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAbilityTask_Repeat_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnFinished,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_Repeat_Statics::NewProp_OnPerformAction,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAbilityTask_Repeat_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAbilityTask_Repeat>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UAbilityTask_Repeat_Statics::ClassParams = {
		&UAbilityTask_Repeat::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAbilityTask_Repeat_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Repeat_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_Repeat_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Repeat_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAbilityTask_Repeat()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UAbilityTask_Repeat_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UAbilityTask_Repeat, 2601210381);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UAbilityTask_Repeat>()
	{
		return UAbilityTask_Repeat::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UAbilityTask_Repeat(Z_Construct_UClass_UAbilityTask_Repeat, &UAbilityTask_Repeat::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UAbilityTask_Repeat"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAbilityTask_Repeat);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
