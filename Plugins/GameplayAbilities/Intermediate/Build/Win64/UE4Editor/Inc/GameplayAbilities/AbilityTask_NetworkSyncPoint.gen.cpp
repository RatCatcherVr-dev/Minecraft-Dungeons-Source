// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAbilityTask_NetworkSyncPoint() {}
// Cross Module References
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EAbilityTaskNetSyncType();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_NetworkSyncPoint();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback();
	GAMEPLAYABILITIES_API UFunction* Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
// End Cross Module References
	struct Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_NetworkSyncPoint.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature_Statics::FuncParams = { (UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities, nullptr, "NetworkSyncDelegate__DelegateSignature", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00130000, 0, 0, METADATA_PARAMS(Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	static UEnum* EAbilityTaskNetSyncType_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_EAbilityTaskNetSyncType, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("EAbilityTaskNetSyncType"));
		}
		return Singleton;
	}
	template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EAbilityTaskNetSyncType>()
	{
		return EAbilityTaskNetSyncType_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EAbilityTaskNetSyncType(EAbilityTaskNetSyncType_StaticEnum, TEXT("/Script/GameplayAbilities"), TEXT("EAbilityTaskNetSyncType"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_GameplayAbilities_EAbilityTaskNetSyncType_Hash() { return 2949616925U; }
	UEnum* Z_Construct_UEnum_GameplayAbilities_EAbilityTaskNetSyncType()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EAbilityTaskNetSyncType"), 0, Get_Z_Construct_UEnum_GameplayAbilities_EAbilityTaskNetSyncType_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "EAbilityTaskNetSyncType::BothWait", (int64)EAbilityTaskNetSyncType::BothWait },
				{ "EAbilityTaskNetSyncType::OnlyServerWait", (int64)EAbilityTaskNetSyncType::OnlyServerWait },
				{ "EAbilityTaskNetSyncType::OnlyClientWait", (int64)EAbilityTaskNetSyncType::OnlyClientWait },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "BothWait.ToolTip", "Both client and server wait until the other signals" },
				{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_NetworkSyncPoint.h" },
				{ "OnlyClientWait.ToolTip", "Only client will wait for the server signal. Server will signal and immediately continue without waiting to hear from Client." },
				{ "OnlyServerWait.ToolTip", "Only server will wait for the client signal. Client will signal and immediately continue without waiting to hear from Server." },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
				nullptr,
				"EAbilityTaskNetSyncType",
				"EAbilityTaskNetSyncType",
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
	void UAbilityTask_NetworkSyncPoint::StaticRegisterNativesUAbilityTask_NetworkSyncPoint()
	{
		UClass* Class = UAbilityTask_NetworkSyncPoint::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "OnSignalCallback", &UAbilityTask_NetworkSyncPoint::execOnSignalCallback },
			{ "WaitNetSync", &UAbilityTask_NetworkSyncPoint::execWaitNetSync },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_NetworkSyncPoint.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAbilityTask_NetworkSyncPoint, nullptr, "OnSignalCallback", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics
	{
		struct AbilityTask_NetworkSyncPoint_eventWaitNetSync_Parms
		{
			UGameplayAbility* OwningAbility;
			EAbilityTaskNetSyncType SyncType;
			UAbilityTask_NetworkSyncPoint* ReturnValue;
		};
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FEnumPropertyParams NewProp_SyncType;
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_SyncType_Underlying;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_OwningAbility;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_NetworkSyncPoint_eventWaitNetSync_Parms, ReturnValue), Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_SyncType = { "SyncType", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_NetworkSyncPoint_eventWaitNetSync_Parms, SyncType), Z_Construct_UEnum_GameplayAbilities_EAbilityTaskNetSyncType, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_SyncType_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_OwningAbility = { "OwningAbility", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(AbilityTask_NetworkSyncPoint_eventWaitNetSync_Parms, OwningAbility), Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_SyncType,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_SyncType_Underlying,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::NewProp_OwningAbility,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::Function_MetaDataParams[] = {
		{ "BlueprintInternalUseOnly", "TRUE" },
		{ "Category", "Ability|Tasks" },
		{ "DefaultToSelf", "OwningAbility" },
		{ "HidePin", "OwningAbility" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_NetworkSyncPoint.h" },
		{ "ToolTip", "Synchronize execution flow between Client and Server. Depending on SyncType, the Client and Server will wait for the other to reach this node or another WaitNetSync node in the ability before continuing execution.\n\nBothWait - Both Client and Server will wait until the other reaches the node. (Whoever gets their first, waits for the other before continueing).\nOnlyServerWait - Only server will wait for the client signal. Client will signal and immediately continue without waiting to hear from Server.\nOnlyClientWait - Only client will wait for the server signal. Server will signal and immediately continue without waiting to hear from Client.\n\nNote that this is \"ability instance wide\". These sync points never affect sync points in other abilities.\n\nIn most cases you will have both client and server execution paths connected to the same WaitNetSync node. However it is possible to use separate nodes\nfor cleanliness of the graph. The \"signal\" is \"ability instance wide\"." },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAbilityTask_NetworkSyncPoint, nullptr, "WaitNetSync", sizeof(AbilityTask_NetworkSyncPoint_eventWaitNetSync_Parms), Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_NoRegister()
	{
		return UAbilityTask_NetworkSyncPoint::StaticClass();
	}
	struct Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OnSync_MetaData[];
#endif
		static const UE4CodeGen_Private::FMulticastDelegatePropertyParams NewProp_OnSync;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UAbilityTask,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_OnSignalCallback, "OnSignalCallback" }, // 1534560003
		{ &Z_Construct_UFunction_UAbilityTask_NetworkSyncPoint_WaitNetSync, "WaitNetSync" }, // 3333181079
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h" },
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_NetworkSyncPoint.h" },
		{ "ToolTip", "Task for providing a generic sync point for client server (one can wait for a signal from the other)" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::NewProp_OnSync_MetaData[] = {
		{ "ModuleRelativePath", "Public/Abilities/Tasks/AbilityTask_NetworkSyncPoint.h" },
	};
#endif
	const UE4CodeGen_Private::FMulticastDelegatePropertyParams Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::NewProp_OnSync = { "OnSync", nullptr, (EPropertyFlags)0x0010000010080000, UE4CodeGen_Private::EPropertyGenFlags::MulticastDelegate, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UAbilityTask_NetworkSyncPoint, OnSync), Z_Construct_UDelegateFunction_GameplayAbilities_NetworkSyncDelegate__DelegateSignature, METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::NewProp_OnSync_MetaData, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::NewProp_OnSync_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::NewProp_OnSync,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAbilityTask_NetworkSyncPoint>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::ClassParams = {
		&UAbilityTask_NetworkSyncPoint::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAbilityTask_NetworkSyncPoint()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UAbilityTask_NetworkSyncPoint_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UAbilityTask_NetworkSyncPoint, 1536277337);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UAbilityTask_NetworkSyncPoint>()
	{
		return UAbilityTask_NetworkSyncPoint::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UAbilityTask_NetworkSyncPoint(Z_Construct_UClass_UAbilityTask_NetworkSyncPoint, &UAbilityTask_NetworkSyncPoint::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UAbilityTask_NetworkSyncPoint"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAbilityTask_NetworkSyncPoint);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
