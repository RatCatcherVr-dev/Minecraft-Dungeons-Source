// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAbilityTask_WaitInputRelease() {}
// Cross Module References
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_WaitInputRelease_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_WaitInputRelease();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
// End Cross Module References
	struct Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics
	{
		struct _Script_GameplayAbilities_eventInputReleaseDelegate_Parms
		{
			float TimeHeld;
		};
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_TimeHeld;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::NewProp_TimeHeld = { "TimeHeld", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(_Script_GameplayAbilities_eventInputReleaseDelegate_Parms, TimeHeld), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::NewProp_TimeHeld,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_WaitInputRelease.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities, nullptr, "InputReleaseDelegate__DelegateSignature", sizeof(_Script_GameplayAbilities_eventInputReleaseDelegate_Parms), Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	void UAbilityTask_WaitInputRelease::StaticRegisterNativesUAbilityTask_WaitInputRelease()
	{
		UClass* Class = UAbilityTask_WaitInputRelease::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "OnReleaseCallback", &UAbilityTask_WaitInputRelease::execOnReleaseCallback },
			{ "WaitInputRelease", &UAbilityTask_WaitInputRelease::execWaitInputRelease },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_WaitInputRelease.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAbilityTask_WaitInputRelease, nullptr, "OnReleaseCallback", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics
	{
		struct AbilityTask_WaitInputRelease_eventWaitInputRelease_Parms
		{
			UGameplayAbility* OwningAbility;
			bool bTestAlreadyReleased;
			UAbilityTask_WaitInputRelease* ReturnValue;
		};
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
		static void NewProp_bTestAlreadyReleased_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bTestAlreadyReleased;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_OwningAbility;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_WaitInputRelease_eventWaitInputRelease_Parms, ReturnValue), Z_Construct_UClass_UAbilityTask_WaitInputRelease_NoRegister, METADATA_PARAMS(nullptr, 0) };
	void Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_bTestAlreadyReleased_SetBit(void* Obj)
	{
		((AbilityTask_WaitInputRelease_eventWaitInputRelease_Parms*)Obj)->bTestAlreadyReleased = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_bTestAlreadyReleased = { "bTestAlreadyReleased", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AbilityTask_WaitInputRelease_eventWaitInputRelease_Parms), &Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_bTestAlreadyReleased_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_OwningAbility = { "OwningAbility", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_WaitInputRelease_eventWaitInputRelease_Parms, OwningAbility), Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_bTestAlreadyReleased,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::NewProp_OwningAbility,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::Function_MetaDataParams[] = {
		{ "BlueprintInternalUseOnly", "TRUE" },
		{ "Category", "Ability|Tasks" },
		{ "CPP_Default_bTestAlreadyReleased", "false" },
		{ "DefaultToSelf", "OwningAbility" },
		{ "HidePin", "OwningAbility" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_WaitInputRelease.h" },
		{ "ToolTip", "Wait until the user releases the input button for this ability's activation. Returns time from hitting this node, till release. Will return 0 if input was already released." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAbilityTask_WaitInputRelease, nullptr, "WaitInputRelease", sizeof(AbilityTask_WaitInputRelease_eventWaitInputRelease_Parms), Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UAbilityTask_WaitInputRelease_NoRegister()
	{
		return UAbilityTask_WaitInputRelease::StaticClass();
	}
	struct Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OnRelease_MetaData[];
#endif
		static const UE4CodeGen_Private::FMulticastDelegatePropertyParams NewProp_OnRelease;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UAbilityTask,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAbilityTask_WaitInputRelease_OnReleaseCallback, "OnReleaseCallback" }, // 1693275811
		{ &Z_Construct_UFunction_UAbilityTask_WaitInputRelease_WaitInputRelease, "WaitInputRelease" }, // 2871146435
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/Tasks/AbilityTask_WaitInputRelease.h" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_WaitInputRelease.h" },
		{ "ToolTip", "Waits until the input is released from activating an ability. Clients will replicate a 'release input' event to the server, but not the exact time it was held locally.\nWe expect server to execute this task in parallel and keep its own time." },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::NewProp_OnRelease_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_WaitInputRelease.h" },
	};
#endif
	const UE4CodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::NewProp_OnRelease = { "OnRelease", nullptr, (EPropertyFlags)0x0010000010080000, UE4CodeGen_Private::EPropertyGenFlags::MulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask_WaitInputRelease, OnRelease), Z_Construct_UDelegateFunction_GameplayAbilities_InputReleaseDelegate__DelegateSignature, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::NewProp_OnRelease_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::NewProp_OnRelease_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::NewProp_OnRelease,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAbilityTask_WaitInputRelease>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::ClassParams = {
		&UAbilityTask_WaitInputRelease::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAbilityTask_WaitInputRelease()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UAbilityTask_WaitInputRelease_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UAbilityTask_WaitInputRelease, 1613828741);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UAbilityTask_WaitInputRelease>()
	{
		return UAbilityTask_WaitInputRelease::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UAbilityTask_WaitInputRelease(Z_Construct_UClass_UAbilityTask_WaitInputRelease, &UAbilityTask_WaitInputRelease::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UAbilityTask_WaitInputRelease"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAbilityTask_WaitInputRelease);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
