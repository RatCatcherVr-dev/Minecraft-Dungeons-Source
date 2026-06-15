// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/Tasks/AbilityTask_StartAbilityState.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAbilityTask_StartAbilityState() {}
// Cross Module References
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_StartAbilityState_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_StartAbilityState();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
// End Cross Module References
	struct Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_StartAbilityState.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities, nullptr, "AbilityStateDelegate__DelegateSignature", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	void UAbilityTask_StartAbilityState::StaticRegisterNativesUAbilityTask_StartAbilityState()
	{
		UClass* Class = UAbilityTask_StartAbilityState::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "StartAbilityState", &UAbilityTask_StartAbilityState::execStartAbilityState },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics
	{
		struct AbilityTask_StartAbilityState_eventStartAbilityState_Parms
		{
			UGameplayAbility* OwningAbility;
			FName StateName;
			bool bEndCurrentState;
			UAbilityTask_StartAbilityState* ReturnValue;
		};
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
		static void NewProp_bEndCurrentState_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bEndCurrentState;
		static const UE4CodeGen_Private::FNamePropertyParams NewProp_StateName;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_OwningAbility;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_StartAbilityState_eventStartAbilityState_Parms, ReturnValue), Z_Construct_UClass_UAbilityTask_StartAbilityState_NoRegister, METADATA_PARAMS(nullptr, 0) };
	void Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_bEndCurrentState_SetBit(void* Obj)
	{
		((AbilityTask_StartAbilityState_eventStartAbilityState_Parms*)Obj)->bEndCurrentState = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_bEndCurrentState = { "bEndCurrentState", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(AbilityTask_StartAbilityState_eventStartAbilityState_Parms), &Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_bEndCurrentState_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FNamePropertyParams Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_StateName = { "StateName", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_StartAbilityState_eventStartAbilityState_Parms, StateName), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_OwningAbility = { "OwningAbility", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_StartAbilityState_eventStartAbilityState_Parms, OwningAbility), Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_bEndCurrentState,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_StateName,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::NewProp_OwningAbility,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::Function_MetaDataParams[] = {
		{ "BlueprintInternalUseOnly", "true" },
		{ "Category", "Ability|Tasks" },
		{ "CPP_Default_bEndCurrentState", "true" },
		{ "DefaultToSelf", "OwningAbility" },
		{ "HidePin", "OwningAbility" },
		{ "HideSpawnParms", "Instigator" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_StartAbilityState.h" },
		{ "ToolTip", "Starts a new ability state.\n\n@param StateName The name of the state.\n@param bEndCurrentState If true, all other active ability states will be ended." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAbilityTask_StartAbilityState, nullptr, "StartAbilityState", sizeof(AbilityTask_StartAbilityState_eventStartAbilityState_Parms), Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UAbilityTask_StartAbilityState_NoRegister()
	{
		return UAbilityTask_StartAbilityState::StaticClass();
	}
	struct Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OnStateInterrupted_MetaData[];
#endif
		static const UE4CodeGen_Private::FMulticastDelegatePropertyParams NewProp_OnStateInterrupted;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OnStateEnded_MetaData[];
#endif
		static const UE4CodeGen_Private::FMulticastDelegatePropertyParams NewProp_OnStateEnded;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UAbilityTask,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAbilityTask_StartAbilityState_StartAbilityState, "StartAbilityState" }, // 173563593
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/Tasks/AbilityTask_StartAbilityState.h" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_StartAbilityState.h" },
		{ "ToolTip", "An ability state is simply an ability task that provides a way to handle the ability being interrupted.\nYou can use ability states to do state-specific cleanup if the ability ends or was interrupted at a certain point during it's execution.\n\nAn ability state will always result in either 'OnStateEnded' or 'OnStateInterrupted' being called.\n\n'OnStateEnded' will be called if:\n- The ability itself ends via AGameplayAbility::EndAbility\n- The ability state is manually ended via AGameplayAbility::EndAbilityState\n- Another ability state is started will 'bEndCurrentState' set to true\n\n'OnStateInterrupted' will be called if:\n- The ability itself is cancelled via AGameplayAbility::CancelAbility" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateInterrupted_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_StartAbilityState.h" },
		{ "ToolTip", "Invoked if the ability was interrupted and this state is active." },
	};
#endif
	const UE4CodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateInterrupted = { "OnStateInterrupted", nullptr, (EPropertyFlags)0x0010000010080000, UE4CodeGen_Private::EPropertyGenFlags::MulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask_StartAbilityState, OnStateInterrupted), Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateInterrupted_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateInterrupted_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateEnded_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_StartAbilityState.h" },
		{ "ToolTip", "Invoked if 'EndAbilityState' is called or if 'EndAbility' is called and this state is active." },
	};
#endif
	const UE4CodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateEnded = { "OnStateEnded", nullptr, (EPropertyFlags)0x0010000010080000, UE4CodeGen_Private::EPropertyGenFlags::MulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask_StartAbilityState, OnStateEnded), Z_Construct_UDelegateFunction_GameplayAbilities_AbilityStateDelegate__DelegateSignature, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateEnded_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateEnded_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateInterrupted,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::NewProp_OnStateEnded,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAbilityTask_StartAbilityState>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::ClassParams = {
		&UAbilityTask_StartAbilityState::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAbilityTask_StartAbilityState()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UAbilityTask_StartAbilityState_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UAbilityTask_StartAbilityState, 3545238073);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UAbilityTask_StartAbilityState>()
	{
		return UAbilityTask_StartAbilityState::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UAbilityTask_StartAbilityState(Z_Construct_UClass_UAbilityTask_StartAbilityState, &UAbilityTask_StartAbilityState::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UAbilityTask_StartAbilityState"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAbilityTask_StartAbilityState);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
