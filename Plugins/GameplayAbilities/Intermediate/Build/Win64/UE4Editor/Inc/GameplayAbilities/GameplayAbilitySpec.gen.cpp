// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayAbilitySpec.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayAbilitySpec() {}
// Cross Module References
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayEffectGrantedAbilityRemovePolicy();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityActivationMode();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializer();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpec();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializerItem();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FActiveGameplayEffectHandle();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject_NoRegister();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FPredictionKey();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecDef();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FScalableFloat();
	COREUOBJECT_API UClass* Z_Construct_UClass_UClass();
// End Cross Module References
	static UEnum* EGameplayEffectGrantedAbilityRemovePolicy_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_EGameplayEffectGrantedAbilityRemovePolicy, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("EGameplayEffectGrantedAbilityRemovePolicy"));
		}
		return Singleton;
	}
	template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EGameplayEffectGrantedAbilityRemovePolicy>()
	{
		return EGameplayEffectGrantedAbilityRemovePolicy_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EGameplayEffectGrantedAbilityRemovePolicy(EGameplayEffectGrantedAbilityRemovePolicy_StaticEnum, TEXT("/Script/GameplayAbilities"), TEXT("EGameplayEffectGrantedAbilityRemovePolicy"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_GameplayAbilities_EGameplayEffectGrantedAbilityRemovePolicy_Hash() { return 1153746081U; }
	UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayEffectGrantedAbilityRemovePolicy()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EGameplayEffectGrantedAbilityRemovePolicy"), 0, Get_Z_Construct_UEnum_GameplayAbilities_EGameplayEffectGrantedAbilityRemovePolicy_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "EGameplayEffectGrantedAbilityRemovePolicy::CancelAbilityImmediately", (int64)EGameplayEffectGrantedAbilityRemovePolicy::CancelAbilityImmediately },
				{ "EGameplayEffectGrantedAbilityRemovePolicy::RemoveAbilityOnEnd", (int64)EGameplayEffectGrantedAbilityRemovePolicy::RemoveAbilityOnEnd },
				{ "EGameplayEffectGrantedAbilityRemovePolicy::DoNothing", (int64)EGameplayEffectGrantedAbilityRemovePolicy::DoNothing },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "CancelAbilityImmediately.ToolTip", "Active abilities are immediately canceled and the ability is removed." },
				{ "DoNothing.ToolTip", "Granted abilities are left lone when the granting GameplayEffect is removed." },
				{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
				{ "RemoveAbilityOnEnd.ToolTip", "Active abilities are allowed to finish, and then removed." },
				{ "ToolTip", "Describes what happens when a GameplayEffect, that is granting an active ability, is removed from its owner." },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
				nullptr,
				"EGameplayEffectGrantedAbilityRemovePolicy",
				"EGameplayEffectGrantedAbilityRemovePolicy",
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
	static UEnum* EGameplayAbilityActivationMode_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityActivationMode, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("EGameplayAbilityActivationMode"));
		}
		return Singleton;
	}
	template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EGameplayAbilityActivationMode::Type>()
	{
		return EGameplayAbilityActivationMode_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EGameplayAbilityActivationMode(EGameplayAbilityActivationMode_StaticEnum, TEXT("/Script/GameplayAbilities"), TEXT("EGameplayAbilityActivationMode"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityActivationMode_Hash() { return 1843662768U; }
	UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityActivationMode()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EGameplayAbilityActivationMode"), 0, Get_Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityActivationMode_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "EGameplayAbilityActivationMode::Authority", (int64)EGameplayAbilityActivationMode::Authority },
				{ "EGameplayAbilityActivationMode::NonAuthority", (int64)EGameplayAbilityActivationMode::NonAuthority },
				{ "EGameplayAbilityActivationMode::Predicting", (int64)EGameplayAbilityActivationMode::Predicting },
				{ "EGameplayAbilityActivationMode::Confirmed", (int64)EGameplayAbilityActivationMode::Confirmed },
				{ "EGameplayAbilityActivationMode::Rejected", (int64)EGameplayAbilityActivationMode::Rejected },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "Authority.ToolTip", "We are the authority activating this ability" },
				{ "BlueprintType", "true" },
				{ "Confirmed.ToolTip", "We are not the authority, but the authority has confirmed this activation" },
				{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
				{ "NonAuthority.ToolTip", "We are not the authority but aren't predicting yet. This is a mostly invalid state to be in" },
				{ "Predicting.ToolTip", "We are predicting the activation of this ability" },
				{ "Rejected.ToolTip", "We tried to activate it, and server told us we couldn't (even though we thought we could)" },
				{ "ToolTip", "Describes the status of activating this ability, this is updated as prediction is handled" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
				nullptr,
				"EGameplayAbilityActivationMode",
				"EGameplayAbilityActivationMode::Type",
				Enumerators,
				ARRAY_COUNT(Enumerators),
				RF_Public|RF_Transient|RF_MarkAsNative,
				UE4CodeGen_Private::EDynamicType::NotDynamic,
				(uint8)UEnum::ECppForm::Namespaced,
				METADATA_PARAMS(Enum_MetaDataParams, ARRAY_COUNT(Enum_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUEnum(ReturnEnum, EnumParams);
		}
		return ReturnEnum;
	}
class UScriptStruct* FGameplayAbilitySpecContainer::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAbilitySpecContainer"), sizeof(FGameplayAbilitySpecContainer), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAbilitySpecContainer>()
{
	return FGameplayAbilitySpecContainer::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAbilitySpecContainer(FGameplayAbilitySpecContainer::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAbilitySpecContainer"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecContainer
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecContainer()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAbilitySpecContainer")),new UScriptStruct::TCppStructOps<FGameplayAbilitySpecContainer>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecContainer;
	struct Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Owner_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Owner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Items_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Items;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Items_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Fast serializer wrapper for above struct" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAbilitySpecContainer>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Owner_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Component that owns this list" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Owner = { "Owner", nullptr, (EPropertyFlags)0x0010000000080008, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecContainer, Owner), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Owner_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Owner_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Items_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "List of activatable abilities" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Items = { "Items", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecContainer, Items), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Items_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Items_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Items_Inner = { "Items", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilitySpec, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Owner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Items,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::NewProp_Items_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		Z_Construct_UScriptStruct_FFastArraySerializer,
		&NewStructOps,
		"GameplayAbilitySpecContainer",
		sizeof(FGameplayAbilitySpecContainer),
		alignof(FGameplayAbilitySpecContainer),
		Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000205),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAbilitySpecContainer"), sizeof(FGameplayAbilitySpecContainer), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecContainer_Hash() { return 3487858988U; }
class UScriptStruct* FGameplayAbilitySpec::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpec_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAbilitySpec, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAbilitySpec"), sizeof(FGameplayAbilitySpec), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpec_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAbilitySpec>()
{
	return FGameplayAbilitySpec::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAbilitySpec(FGameplayAbilitySpec::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAbilitySpec"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpec
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpec()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAbilitySpec")),new UScriptStruct::TCppStructOps<FGameplayAbilitySpec>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpec;
	struct Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_GameplayEffectHandle_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_GameplayEffectHandle;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReplicatedInstances_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_ReplicatedInstances;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_ReplicatedInstances_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NonReplicatedInstances_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NonReplicatedInstances;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NonReplicatedInstances_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ActivationInfo_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_ActivationInfo;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PendingRemove_MetaData[];
#endif
		static void NewProp_PendingRemove_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_PendingRemove;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_RemoveAfterActivation_MetaData[];
#endif
		static void NewProp_RemoveAfterActivation_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_RemoveAfterActivation;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_InputPressed_MetaData[];
#endif
		static void NewProp_InputPressed_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_InputPressed;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ActiveCount_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_ActiveCount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SourceObject_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SourceObject;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_InputID_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_InputID;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Level_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_Level;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Ability_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Ability;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Handle_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Handle;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "An activatable ability spec, hosted on the ability system component. This defines both what the ability is (what class, what level, input binding etc)\nand also holds runtime state that must be kept outside of the ability being instanced/activated." },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAbilitySpec>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_GameplayEffectHandle_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Handle to GE that granted us (usually invalid)" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_GameplayEffectHandle = { "GameplayEffectHandle", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, GameplayEffectHandle), Z_Construct_UScriptStruct_FActiveGameplayEffectHandle, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_GameplayEffectHandle_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_GameplayEffectHandle_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ReplicatedInstances_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Replicated instances of this ability.." },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ReplicatedInstances = { "ReplicatedInstances", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, ReplicatedInstances), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ReplicatedInstances_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ReplicatedInstances_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ReplicatedInstances_Inner = { "ReplicatedInstances", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_NonReplicatedInstances_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Non replicating instances of this ability." },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_NonReplicatedInstances = { "NonReplicatedInstances", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, NonReplicatedInstances), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_NonReplicatedInstances_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_NonReplicatedInstances_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_NonReplicatedInstances_Inner = { "NonReplicatedInstances", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActivationInfo_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Activation state of this ability. This is not replicated since it needs to be overwritten locally on clients during prediction." },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActivationInfo = { "ActivationInfo", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, ActivationInfo), Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActivationInfo_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActivationInfo_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_PendingRemove_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Pending removal due to scope lock" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_PendingRemove_SetBit(void* Obj)
	{
		((FGameplayAbilitySpec*)Obj)->PendingRemove = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_PendingRemove = { "PendingRemove", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Bool , RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(uint8), sizeof(FGameplayAbilitySpec), &Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_PendingRemove_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_PendingRemove_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_PendingRemove_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_RemoveAfterActivation_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "If true, this ability should be removed as soon as it finishes executing" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_RemoveAfterActivation_SetBit(void* Obj)
	{
		((FGameplayAbilitySpec*)Obj)->RemoveAfterActivation = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_RemoveAfterActivation = { "RemoveAfterActivation", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Bool , RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(uint8), sizeof(FGameplayAbilitySpec), &Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_RemoveAfterActivation_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_RemoveAfterActivation_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_RemoveAfterActivation_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputPressed_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Is input currently pressed. Set to false when input is released" },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputPressed_SetBit(void* Obj)
	{
		((FGameplayAbilitySpec*)Obj)->InputPressed = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputPressed = { "InputPressed", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Bool , RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(uint8), sizeof(FGameplayAbilitySpec), &Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputPressed_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputPressed_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputPressed_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActiveCount_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "A count of the number of times this ability has been activated minus the number of times it has been ended. For instanced abilities this will be the number of currently active instances. Can't replicate until prediction accurately handles this." },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActiveCount = { "ActiveCount", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, ActiveCount), nullptr, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActiveCount_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActiveCount_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_SourceObject_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Object this ability was created from, can be an actor or static object. Useful to bind an ability to a gameplay object" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_SourceObject = { "SourceObject", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, SourceObject), Z_Construct_UClass_UObject_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_SourceObject_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_SourceObject_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputID_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "InputID, if bound" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputID = { "InputID", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, InputID), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputID_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputID_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Level_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Level of Ability" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Level = { "Level", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, Level), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Level_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Level_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Ability_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Ability of the spec (Always the CDO. This should be const but too many things modify it currently)" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Ability = { "Ability", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, Ability), Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Ability_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Ability_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Handle_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Handle for outside sources to refer to this spec by" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Handle = { "Handle", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpec, Handle), Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Handle_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Handle_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_GameplayEffectHandle,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ReplicatedInstances,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ReplicatedInstances_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_NonReplicatedInstances,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_NonReplicatedInstances_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActivationInfo,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_PendingRemove,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_RemoveAfterActivation,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputPressed,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_ActiveCount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_SourceObject,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_InputID,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Level,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Ability,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::NewProp_Handle,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		Z_Construct_UScriptStruct_FFastArraySerializerItem,
		&NewStructOps,
		"GameplayAbilitySpec",
		sizeof(FGameplayAbilitySpec),
		alignof(FGameplayAbilitySpec),
		Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpec()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpec_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAbilitySpec"), sizeof(FGameplayAbilitySpec), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpec_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAbilitySpec_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpec_Hash() { return 2073006190U; }
class UScriptStruct* FGameplayAbilityActivationInfo::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAbilityActivationInfo"), sizeof(FGameplayAbilityActivationInfo), Get_Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAbilityActivationInfo>()
{
	return FGameplayAbilityActivationInfo::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAbilityActivationInfo(FGameplayAbilityActivationInfo::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAbilityActivationInfo"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilityActivationInfo
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilityActivationInfo()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAbilityActivationInfo")),new UScriptStruct::TCppStructOps<FGameplayAbilityActivationInfo>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilityActivationInfo;
	struct Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PredictionKeyWhenActivated_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_PredictionKeyWhenActivated;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bCanBeEndedByOtherInstance_MetaData[];
#endif
		static void NewProp_bCanBeEndedByOtherInstance_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bCanBeEndedByOtherInstance;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ActivationMode_MetaData[];
#endif
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_ActivationMode;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "FGameplayAbilityActivationInfo\n\nData tied to a specific activation of an ability.\n        -Tell us whether we are the authority, if we are predicting, confirmed, etc.\n        -Holds current and previous PredictionKey\n        -Generally not meant to be subclassed in projects.\n        -Passed around by value since the struct is small." },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAbilityActivationInfo>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_PredictionKeyWhenActivated_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "This was the prediction key used to activate this ability. It does not get updated if new prediction keys are generated over the course of the ability" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_PredictionKeyWhenActivated = { "PredictionKeyWhenActivated", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilityActivationInfo, PredictionKeyWhenActivated), Z_Construct_UScriptStruct_FPredictionKey, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_PredictionKeyWhenActivated_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_PredictionKeyWhenActivated_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_bCanBeEndedByOtherInstance_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "An ability that runs on multiple game instances can be canceled by a remote instance, but only if that remote instance has already confirmed starting it." },
	};
#endif
	void Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_bCanBeEndedByOtherInstance_SetBit(void* Obj)
	{
		((FGameplayAbilityActivationInfo*)Obj)->bCanBeEndedByOtherInstance = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_bCanBeEndedByOtherInstance = { "bCanBeEndedByOtherInstance", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Bool , RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(uint8), sizeof(FGameplayAbilityActivationInfo), &Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_bCanBeEndedByOtherInstance_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_bCanBeEndedByOtherInstance_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_bCanBeEndedByOtherInstance_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_ActivationMode_MetaData[] = {
		{ "Category", "ActorInfo" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Activation status of this ability" },
	};
#endif
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_ActivationMode = { "ActivationMode", nullptr, (EPropertyFlags)0x0010000000000014, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilityActivationInfo, ActivationMode), Z_Construct_UEnum_GameplayAbilities_EGameplayAbilityActivationMode, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_ActivationMode_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_ActivationMode_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_PredictionKeyWhenActivated,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_bCanBeEndedByOtherInstance,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::NewProp_ActivationMode,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayAbilityActivationInfo",
		sizeof(FGameplayAbilityActivationInfo),
		alignof(FGameplayAbilityActivationInfo),
		Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAbilityActivationInfo"), sizeof(FGameplayAbilityActivationInfo), Get_Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilityActivationInfo_Hash() { return 2068633824U; }
class UScriptStruct* FGameplayAbilitySpecDef::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAbilitySpecDef"), sizeof(FGameplayAbilitySpecDef), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAbilitySpecDef>()
{
	return FGameplayAbilitySpecDef::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAbilitySpecDef(FGameplayAbilitySpecDef::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAbilitySpecDef"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecDef
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecDef()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAbilitySpecDef")),new UScriptStruct::TCppStructOps<FGameplayAbilitySpecDef>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecDef;
	struct Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AssignedHandle_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_AssignedHandle;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SourceObject_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SourceObject;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_RemovalPolicy_MetaData[];
#endif
		static const UE4CodeGen_Private::FEnumPropertyParams NewProp_RemovalPolicy;
		static const UE4CodeGen_Private::FBytePropertyParams NewProp_RemovalPolicy_Underlying;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_InputID_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_InputID;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LevelScalableFloat_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_LevelScalableFloat;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Ability_MetaData[];
#endif
		static const UE4CodeGen_Private::FClassPropertyParams NewProp_Ability;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "This is data that can be used to create an FGameplayAbilitySpec. Has some data that is only relevant when granted by a GameplayEffect" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAbilitySpecDef>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_AssignedHandle_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "This handle can be set if the SpecDef is used to create a real FGameplaybilitySpec" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_AssignedHandle = { "AssignedHandle", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecDef, AssignedHandle), Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_AssignedHandle_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_AssignedHandle_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_SourceObject_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "What granted this spec, not replicated or settable in editor" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_SourceObject = { "SourceObject", nullptr, (EPropertyFlags)0x0010000080000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecDef, SourceObject), Z_Construct_UClass_UObject_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_SourceObject_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_SourceObject_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_RemovalPolicy_MetaData[] = {
		{ "Category", "Ability Definition" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "What will remove this ability later" },
	};
#endif
	const UE4CodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_RemovalPolicy = { "RemovalPolicy", nullptr, (EPropertyFlags)0x0010000080010001, UE4CodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecDef, RemovalPolicy), Z_Construct_UEnum_GameplayAbilities_EGameplayEffectGrantedAbilityRemovePolicy, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_RemovalPolicy_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_RemovalPolicy_MetaData)) };
	const UE4CodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_RemovalPolicy_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_InputID_MetaData[] = {
		{ "Category", "Ability Definition" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Input ID to bind this ability to" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_InputID = { "InputID", nullptr, (EPropertyFlags)0x0010000080010001, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecDef, InputID), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_InputID_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_InputID_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_LevelScalableFloat_MetaData[] = {
		{ "Category", "Ability Definition" },
		{ "DisplayName", "Level" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "What level to grant this ability at" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_LevelScalableFloat = { "LevelScalableFloat", nullptr, (EPropertyFlags)0x0010000080010001, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecDef, LevelScalableFloat), Z_Construct_UScriptStruct_FScalableFloat, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_LevelScalableFloat_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_LevelScalableFloat_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_Ability_MetaData[] = {
		{ "Category", "Ability Definition" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "What ability to grant" },
	};
#endif
	const UE4CodeGen_Private::FClassPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_Ability = { "Ability", nullptr, (EPropertyFlags)0x0014000080010001, UE4CodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecDef, Ability), Z_Construct_UClass_UGameplayAbility_NoRegister, Z_Construct_UClass_UClass, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_Ability_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_Ability_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_AssignedHandle,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_SourceObject,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_RemovalPolicy,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_RemovalPolicy_Underlying,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_InputID,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_LevelScalableFloat,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::NewProp_Ability,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayAbilitySpecDef",
		sizeof(FGameplayAbilitySpecDef),
		alignof(FGameplayAbilitySpecDef),
		Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecDef()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAbilitySpecDef"), sizeof(FGameplayAbilitySpecDef), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecDef_Hash() { return 671706420U; }
class UScriptStruct* FGameplayAbilitySpecHandle::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAbilitySpecHandle"), sizeof(FGameplayAbilitySpecHandle), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAbilitySpecHandle>()
{
	return FGameplayAbilitySpecHandle::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAbilitySpecHandle(FGameplayAbilitySpecHandle::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAbilitySpecHandle"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecHandle
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecHandle()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAbilitySpecHandle")),new UScriptStruct::TCppStructOps<FGameplayAbilitySpecHandle>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAbilitySpecHandle;
	struct Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Handle_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_Handle;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
		{ "ToolTip", "Handle that points to a specific granted ability. These are globally unique" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAbilitySpecHandle>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::NewProp_Handle_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayAbilitySpec.h" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::NewProp_Handle = { "Handle", nullptr, (EPropertyFlags)0x0040000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAbilitySpecHandle, Handle), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::NewProp_Handle_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::NewProp_Handle_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::NewProp_Handle,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayAbilitySpecHandle",
		sizeof(FGameplayAbilitySpecHandle),
		alignof(FGameplayAbilitySpecHandle),
		Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAbilitySpecHandle"), sizeof(FGameplayAbilitySpecHandle), Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle_Hash() { return 1659041825U; }
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
