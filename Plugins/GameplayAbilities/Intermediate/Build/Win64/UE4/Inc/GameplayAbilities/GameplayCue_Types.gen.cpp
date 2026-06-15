// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayCue_Types.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayCue_Types() {}
// Cross Module References
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayCuePayloadType();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FPreallocationInfo();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_AGameplayCueNotify_Actor_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCuePendingExecute();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayCueParameters();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEffectSpecForRPC();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FPredictionKey();
// End Cross Module References
	static UEnum* EGameplayCuePayloadType_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_EGameplayCuePayloadType, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("EGameplayCuePayloadType"));
		}
		return Singleton;
	}
	template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EGameplayCuePayloadType>()
	{
		return EGameplayCuePayloadType_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EGameplayCuePayloadType(EGameplayCuePayloadType_StaticEnum, TEXT("/Script/GameplayAbilities"), TEXT("EGameplayCuePayloadType"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_GameplayAbilities_EGameplayCuePayloadType_Hash() { return 777497759U; }
	UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayCuePayloadType()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EGameplayCuePayloadType"), 0, Get_Z_Construct_UEnum_GameplayAbilities_EGameplayCuePayloadType_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "EGameplayCuePayloadType::EffectContext", (int64)EGameplayCuePayloadType::EffectContext },
				{ "EGameplayCuePayloadType::CueParameters", (int64)EGameplayCuePayloadType::CueParameters },
				{ "EGameplayCuePayloadType::FromSpec", (int64)EGameplayCuePayloadType::FromSpec },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "CueParameters.ToolTip", "Uses FGameplayCueParameters" },
				{ "EffectContext.ToolTip", "Uses FGameplayEffectContextHandle inside GameplayCueParameters" },
				{ "FromSpec.ToolTip", "Uses FGameplayEffectSpecForRPC" },
				{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
				{ "ToolTip", "Describes what type of payload is attached to a cue execution, we only replicate what is needed" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
				nullptr,
				"EGameplayCuePayloadType",
				"EGameplayCuePayloadType",
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
class UScriptStruct* FPreallocationInfo::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FPreallocationInfo_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FPreallocationInfo, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("PreallocationInfo"), sizeof(FPreallocationInfo), Get_Z_Construct_UScriptStruct_FPreallocationInfo_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FPreallocationInfo>()
{
	return FPreallocationInfo::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FPreallocationInfo(FPreallocationInfo::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("PreallocationInfo"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFPreallocationInfo
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFPreallocationInfo()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("PreallocationInfo")),new UScriptStruct::TCppStructOps<FPreallocationInfo>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFPreallocationInfo;
	struct Z_Construct_UScriptStruct_FPreallocationInfo_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ClassesNeedingPreallocation_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_ClassesNeedingPreallocation;
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_ClassesNeedingPreallocation_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPreallocationInfo_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "Struct for pooling and preallocating gameplaycuenotify_actor classes. This data is per world and used to track what actors are available to recycle and which classes need to preallocate instances of those actors" },
	};
#endif
	void* Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FPreallocationInfo>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewProp_ClassesNeedingPreallocation_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "List of calsses that will be pooled" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewProp_ClassesNeedingPreallocation = { "ClassesNeedingPreallocation", nullptr, (EPropertyFlags)0x0014000000002000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FPreallocationInfo, ClassesNeedingPreallocation), METADATA_PARAMS(Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewProp_ClassesNeedingPreallocation_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewProp_ClassesNeedingPreallocation_MetaData)) };
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewProp_ClassesNeedingPreallocation_Inner = { "ClassesNeedingPreallocation", nullptr, (EPropertyFlags)0x0004000000000000, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_AGameplayCueNotify_Actor_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FPreallocationInfo_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewProp_ClassesNeedingPreallocation,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPreallocationInfo_Statics::NewProp_ClassesNeedingPreallocation_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FPreallocationInfo_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"PreallocationInfo",
		sizeof(FPreallocationInfo),
		alignof(FPreallocationInfo),
		Z_Construct_UScriptStruct_FPreallocationInfo_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FPreallocationInfo_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FPreallocationInfo_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FPreallocationInfo_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FPreallocationInfo()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FPreallocationInfo_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("PreallocationInfo"), sizeof(FPreallocationInfo), Get_Z_Construct_UScriptStruct_FPreallocationInfo_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FPreallocationInfo_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FPreallocationInfo_Hash() { return 1127132635U; }
class UScriptStruct* FGameplayCuePendingExecute::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayCuePendingExecute, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayCuePendingExecute"), sizeof(FGameplayCuePendingExecute), Get_Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayCuePendingExecute>()
{
	return FGameplayCuePendingExecute::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayCuePendingExecute(FGameplayCuePendingExecute::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayCuePendingExecute"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCuePendingExecute
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCuePendingExecute()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayCuePendingExecute")),new UScriptStruct::TCppStructOps<FGameplayCuePendingExecute>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayCuePendingExecute;
	struct Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CueParameters_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_CueParameters;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FromSpec_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_FromSpec;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_OwningComponent_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_OwningComponent;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PayloadType_MetaData[];
#endif
		static const UE4CodeGen_Private::FEnumPropertyParams NewProp_PayloadType;
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_PayloadType_Underlying;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PredictionKey_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_PredictionKey;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "Structure to keep track of pending gameplay cues that haven't been applied yet." },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayCuePendingExecute>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_CueParameters_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "Store the full cue parameters or just the effect context depending on type" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_CueParameters = { "CueParameters", nullptr, (EPropertyFlags)0x0010008000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCuePendingExecute, CueParameters), Z_Construct_UScriptStruct_FGameplayCueParameters, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_CueParameters_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_CueParameters_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_FromSpec_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "If this cue is from a spec, here's the copy of that spec" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_FromSpec = { "FromSpec", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCuePendingExecute, FromSpec), Z_Construct_UScriptStruct_FGameplayEffectSpecForRPC, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_FromSpec_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_FromSpec_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_OwningComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "What component to send the cue on" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_OwningComponent = { "OwningComponent", nullptr, (EPropertyFlags)0x0010000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCuePendingExecute, OwningComponent), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_OwningComponent_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_OwningComponent_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PayloadType_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "What type of payload is attached to this cue" },
	};
#endif
	const UE4CodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PayloadType = { "PayloadType", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCuePendingExecute, PayloadType), Z_Construct_UEnum_GameplayAbilities_EGameplayCuePayloadType, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PayloadType_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PayloadType_MetaData)) };
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PayloadType_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PredictionKey_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayCue_Types.h" },
		{ "ToolTip", "Prediction key that spawned this cue" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PredictionKey = { "PredictionKey", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayCuePendingExecute, PredictionKey), Z_Construct_UScriptStruct_FPredictionKey, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PredictionKey_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PredictionKey_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_CueParameters,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_FromSpec,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_OwningComponent,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PayloadType,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PayloadType_Underlying,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::NewProp_PredictionKey,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayCuePendingExecute",
		sizeof(FGameplayCuePendingExecute),
		alignof(FGameplayCuePendingExecute),
		Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000005),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayCuePendingExecute()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayCuePendingExecute"), sizeof(FGameplayCuePendingExecute), Get_Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayCuePendingExecute_Hash() { return 1167614412U; }
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
