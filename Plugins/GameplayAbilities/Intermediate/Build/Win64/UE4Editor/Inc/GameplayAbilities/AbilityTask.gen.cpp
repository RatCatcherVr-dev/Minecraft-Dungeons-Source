// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/Tasks/AbilityTask.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAbilityTask() {}
// Cross Module References
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EAbilityTaskWaitState();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask();
	GAMEPLAYTASKS_API UClass* Z_Construct_UClass_UGameplayTask();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
// End Cross Module References
	static UEnum* EAbilityTaskWaitState_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_EAbilityTaskWaitState, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("EAbilityTaskWaitState"));
		}
		return Singleton;
	}
	template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EAbilityTaskWaitState>()
	{
		return EAbilityTaskWaitState_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EAbilityTaskWaitState(EAbilityTaskWaitState_StaticEnum, TEXT("/Script/GameplayAbilities"), TEXT("EAbilityTaskWaitState"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_GameplayAbilities_EAbilityTaskWaitState_Hash() { return 592562444U; }
	UEnum* Z_Construct_UEnum_GameplayAbilities_EAbilityTaskWaitState()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EAbilityTaskWaitState"), 0, Get_Z_Construct_UEnum_GameplayAbilities_EAbilityTaskWaitState_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "EAbilityTaskWaitState::WaitingOnGame", (int64)EAbilityTaskWaitState::WaitingOnGame },
				{ "EAbilityTaskWaitState::WaitingOnUser", (int64)EAbilityTaskWaitState::WaitingOnUser },
				{ "EAbilityTaskWaitState::WaitingOnAvatar", (int64)EAbilityTaskWaitState::WaitingOnAvatar },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask.h" },
				{ "ToolTip", "Latent tasks are waiting on something. This is to differeniate waiting on the user to do something vs waiting on the game to do something.\nTasks start WaitingOnGame, and are set to WaitingOnUser when appropriate (see WaitTargetData, WaitIiputPress, etc)" },
				{ "WaitingOnAvatar.ToolTip", "Waiting on Avatar (Character/Pawn/Actor) to do something (usually something physical in the world, like land, move, etc)" },
				{ "WaitingOnGame.ToolTip", "Task is waiting for the game to do something" },
				{ "WaitingOnUser.ToolTip", "Waiting for the user to do something" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
				nullptr,
				"EAbilityTaskWaitState",
				"EAbilityTaskWaitState",
				Enumerators,
				ARRAY_COUNT(Enumerators),
				RF_Public|RF_Transient|RF_MarkAsNative,
				UE4CodeGen_Private::EDynamicType::NotDynamic,
				(uint8)UEnum::ECppForm::EnumClass,
				METADATA_PARAMS(Enum_MetaDataParams, ARRAY_COUNT(Enum_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUEnum(ReturnEnum, EnumParams);
		}
		return ReturnEnum;
	}
	void UAbilityTask::StaticRegisterNativesUAbilityTask()
	{
	}
	UClass* Z_Construct_UClass_UAbilityTask_NoRegister()
	{
		return UAbilityTask::StaticClass();
	}
	struct Z_Construct_UClass_UAbilityTask_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AbilitySystemComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_AbilitySystemComponent;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Ability_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Ability;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAbilityTask_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UGameplayTask,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/Tasks/AbilityTask.h" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_Statics::NewProp_AbilitySystemComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAbilityTask_Statics::NewProp_AbilitySystemComponent = { "AbilitySystemComponent", nullptr, (EPropertyFlags)0x0010000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask, AbilitySystemComponent), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_Statics::NewProp_AbilitySystemComponent_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Statics::NewProp_AbilitySystemComponent_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_Statics::NewProp_Ability_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask.h" },
		{ "ToolTip", "GameplayAbility that created us" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAbilityTask_Statics::NewProp_Ability = { "Ability", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask, Ability), Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_Statics::NewProp_Ability_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Statics::NewProp_Ability_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAbilityTask_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_Statics::NewProp_AbilitySystemComponent,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_Statics::NewProp_Ability,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAbilityTask_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAbilityTask>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UAbilityTask_Statics::ClassParams = {
		&UAbilityTask::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UAbilityTask_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Statics::PropPointers),
		0,
		0x009000A5u,
		METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAbilityTask()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UAbilityTask_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UAbilityTask, 437993336);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UAbilityTask>()
	{
		return UAbilityTask::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UAbilityTask(Z_Construct_UClass_UAbilityTask, &UAbilityTask::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UAbilityTask"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAbilityTask);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
