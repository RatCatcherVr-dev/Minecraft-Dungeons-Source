// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/AttributeSet.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAttributeSet() {}
// Cross Module References
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FAttributeMetaData();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FTableRowBase();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FScalableFloat();
	ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FCurveTableRowHandle();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttribute();
	COREUOBJECT_API UClass* Z_Construct_UClass_UStruct();
	COREUOBJECT_API UClass* Z_Construct_UClass_UProperty();
	GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttributeData();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAttributeSet_NoRegister();
	GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAttributeSet();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
// End Cross Module References
class UScriptStruct* FAttributeMetaData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FAttributeMetaData_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FAttributeMetaData, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("AttributeMetaData"), sizeof(FAttributeMetaData), Get_Z_Construct_UScriptStruct_FAttributeMetaData_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FAttributeMetaData>()
{
	return FAttributeMetaData::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FAttributeMetaData(FAttributeMetaData::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("AttributeMetaData"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFAttributeMetaData
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFAttributeMetaData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("AttributeMetaData")),new UScriptStruct::TCppStructOps<FAttributeMetaData>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFAttributeMetaData;
	struct Z_Construct_UScriptStruct_FAttributeMetaData_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bCanStack_MetaData[];
#endif
		static void NewProp_bCanStack_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bCanStack;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_DerivedAttributeInfo_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_DerivedAttributeInfo;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaxValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MaxValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MinValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MinValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_BaseValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_BaseValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAttributeMetaData_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "DataTable that allows us to define meta data about attributes. Still a work in progress." },
	};
#endif
	void* Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FAttributeMetaData>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_bCanStack_MetaData[] = {
		{ "Category", "Gameplay Attribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	void Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_bCanStack_SetBit(void* Obj)
	{
		((FAttributeMetaData*)Obj)->bCanStack = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_bCanStack = { "bCanStack", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FAttributeMetaData), &Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_bCanStack_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_bCanStack_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_bCanStack_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_DerivedAttributeInfo_MetaData[] = {
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_DerivedAttributeInfo = { "DerivedAttributeInfo", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FAttributeMetaData, DerivedAttributeInfo), METADATA_PARAMS(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_DerivedAttributeInfo_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_DerivedAttributeInfo_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MaxValue_MetaData[] = {
		{ "Category", "Gameplay Attribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MaxValue = { "MaxValue", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FAttributeMetaData, MaxValue), METADATA_PARAMS(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MaxValue_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MaxValue_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MinValue_MetaData[] = {
		{ "Category", "Gameplay Attribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MinValue = { "MinValue", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FAttributeMetaData, MinValue), METADATA_PARAMS(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MinValue_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MinValue_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_BaseValue_MetaData[] = {
		{ "Category", "Gameplay Attribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_BaseValue = { "BaseValue", nullptr, (EPropertyFlags)0x0010000000000015, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FAttributeMetaData, BaseValue), METADATA_PARAMS(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_BaseValue_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_BaseValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FAttributeMetaData_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_bCanStack,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_DerivedAttributeInfo,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MaxValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_MinValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAttributeMetaData_Statics::NewProp_BaseValue,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FAttributeMetaData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		Z_Construct_UScriptStruct_FTableRowBase,
		&NewStructOps,
		"AttributeMetaData",
		sizeof(FAttributeMetaData),
		alignof(FAttributeMetaData),
		Z_Construct_UScriptStruct_FAttributeMetaData_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FAttributeMetaData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FAttributeMetaData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FAttributeMetaData_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("AttributeMetaData"), sizeof(FAttributeMetaData), Get_Z_Construct_UScriptStruct_FAttributeMetaData_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FAttributeMetaData_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FAttributeMetaData_Hash() { return 1317556831U; }
class UScriptStruct* FScalableFloat::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FScalableFloat_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FScalableFloat, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ScalableFloat"), sizeof(FScalableFloat), Get_Z_Construct_UScriptStruct_FScalableFloat_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FScalableFloat>()
{
	return FScalableFloat::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FScalableFloat(FScalableFloat::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("ScalableFloat"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFScalableFloat
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFScalableFloat()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("ScalableFloat")),new UScriptStruct::TCppStructOps<FScalableFloat>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFScalableFloat;
	struct Z_Construct_UScriptStruct_FScalableFloat_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Curve_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Curve;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Value_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Value;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FScalableFloat_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "Generic numerical value in the form Value * Curve[Level]" },
	};
#endif
	void* Z_Construct_UScriptStruct_FScalableFloat_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FScalableFloat>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Curve_MetaData[] = {
		{ "Category", "ScalableFloat" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "Curve that is evaluated at a specific level. If found, it is multipled by Value" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Curve = { "Curve", nullptr, (EPropertyFlags)0x0010000000000005, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FScalableFloat, Curve), Z_Construct_UScriptStruct_FCurveTableRowHandle, METADATA_PARAMS(Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Curve_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Curve_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Value_MetaData[] = {
		{ "Category", "ScalableFloat" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "Raw value, is multiplied by curve" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Value = { "Value", nullptr, (EPropertyFlags)0x0010000000000005, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FScalableFloat, Value), METADATA_PARAMS(Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Value_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Value_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FScalableFloat_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Curve,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FScalableFloat_Statics::NewProp_Value,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FScalableFloat_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"ScalableFloat",
		sizeof(FScalableFloat),
		alignof(FScalableFloat),
		Z_Construct_UScriptStruct_FScalableFloat_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FScalableFloat_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FScalableFloat_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FScalableFloat_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FScalableFloat()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FScalableFloat_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("ScalableFloat"), sizeof(FScalableFloat), Get_Z_Construct_UScriptStruct_FScalableFloat_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FScalableFloat_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FScalableFloat_Hash() { return 4078874193U; }
class UScriptStruct* FGameplayAttribute::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAttribute_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAttribute, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAttribute"), sizeof(FGameplayAttribute), Get_Z_Construct_UScriptStruct_FGameplayAttribute_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAttribute>()
{
	return FGameplayAttribute::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAttribute(FGameplayAttribute::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAttribute"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAttribute
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAttribute()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAttribute")),new UScriptStruct::TCppStructOps<FGameplayAttribute>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAttribute;
	struct Z_Construct_UScriptStruct_FGameplayAttribute_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AttributeOwner_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_AttributeOwner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Attribute_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Attribute;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_AttributeName_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_AttributeName;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAttribute_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "Describes a FGameplayAttributeData or float property inside an attribute set. Using this provides editor UI and gelper functions" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAttribute>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeOwner_MetaData[] = {
		{ "Category", "GameplayAttribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeOwner = { "AttributeOwner", nullptr, (EPropertyFlags)0x0040000000020001, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAttribute, AttributeOwner), Z_Construct_UClass_UStruct, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeOwner_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeOwner_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_Attribute_MetaData[] = {
		{ "Category", "GameplayAttribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_Attribute = { "Attribute", nullptr, (EPropertyFlags)0x0040000000000001, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAttribute, Attribute), Z_Construct_UClass_UProperty, METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_Attribute_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_Attribute_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeName_MetaData[] = {
		{ "Category", "GameplayAttribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "Name of the attribute, usually the same as property name" },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeName = { "AttributeName", nullptr, (EPropertyFlags)0x0010000000020015, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAttribute, AttributeName), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeName_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeName_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAttribute_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeOwner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_Attribute,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAttribute_Statics::NewProp_AttributeName,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAttribute_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayAttribute",
		sizeof(FGameplayAttribute),
		alignof(FGameplayAttribute),
		Z_Construct_UScriptStruct_FGameplayAttribute_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttribute_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttribute()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAttribute_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAttribute"), sizeof(FGameplayAttribute), Get_Z_Construct_UScriptStruct_FGameplayAttribute_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAttribute_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAttribute_Hash() { return 157637944U; }
class UScriptStruct* FGameplayAttributeData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GAMEPLAYABILITIES_API uint32 Get_Z_Construct_UScriptStruct_FGameplayAttributeData_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FGameplayAttributeData, Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("GameplayAttributeData"), sizeof(FGameplayAttributeData), Get_Z_Construct_UScriptStruct_FGameplayAttributeData_Hash());
	}
	return Singleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FGameplayAttributeData>()
{
	return FGameplayAttributeData::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FGameplayAttributeData(FGameplayAttributeData::StaticStruct, TEXT("/Script/GameplayAbilities"), TEXT("GameplayAttributeData"), false, nullptr, nullptr);
static struct FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAttributeData
{
	FScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAttributeData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("GameplayAttributeData")),new UScriptStruct::TCppStructOps<FGameplayAttributeData>);
	}
} ScriptStruct_GameplayAbilities_StaticRegisterNativesFGameplayAttributeData;
	struct Z_Construct_UScriptStruct_FGameplayAttributeData_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CurrentValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_CurrentValue;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_BaseValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_BaseValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "Place in an AttributeSet to create an attribute that can be accesed using FGameplayAttribute. It is strongly encouraged to use this instead of raw float attributes" },
	};
#endif
	void* Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGameplayAttributeData>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_CurrentValue_MetaData[] = {
		{ "Category", "Attribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_CurrentValue = { "CurrentValue", nullptr, (EPropertyFlags)0x0020080000000014, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAttributeData, CurrentValue), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_CurrentValue_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_CurrentValue_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_BaseValue_MetaData[] = {
		{ "Category", "Attribute" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_BaseValue = { "BaseValue", nullptr, (EPropertyFlags)0x0020080000000014, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FGameplayAttributeData, BaseValue), METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_BaseValue_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_BaseValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_CurrentValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::NewProp_BaseValue,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
		nullptr,
		&NewStructOps,
		"GameplayAttributeData",
		sizeof(FGameplayAttributeData),
		alignof(FGameplayAttributeData),
		Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttributeData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FGameplayAttributeData_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_GameplayAbilities();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("GameplayAttributeData"), sizeof(FGameplayAttributeData), Get_Z_Construct_UScriptStruct_FGameplayAttributeData_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FGameplayAttributeData_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FGameplayAttributeData_Hash() { return 2575030103U; }
	void UAttributeSet::StaticRegisterNativesUAttributeSet()
	{
	}
	UClass* Z_Construct_UClass_UAttributeSet_NoRegister()
	{
		return UAttributeSet::StaticClass();
	}
	struct Z_Construct_UClass_UAttributeSet_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAttributeSet_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAttributeSet_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "AttributeSet.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/AttributeSet.h" },
		{ "ToolTip", "Defines the set of all GameplayAttributes for your game\nGames should subclass this and add FGameplayAttributeData properties to represent attributes like health, damage, etc\nAttributeSets are added to the actors as subobjects, and then registered with the AbilitySystemComponent\nIt often desired to have several sets per project that inherit from each other\nYou could make a base health set, then have a player set that inherits from it and adds more attributes" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAttributeSet_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAttributeSet>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UAttributeSet_Statics::ClassParams = {
		&UAttributeSet::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x003000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UAttributeSet_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UAttributeSet_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAttributeSet()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UAttributeSet_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UAttributeSet, 3465423835);
	template<> GAMEPLAYABILITIES_API UClass* StaticClass<UAttributeSet>()
	{
		return UAttributeSet::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UAttributeSet(Z_Construct_UClass_UAttributeSet, &UAttributeSet::StaticClass, TEXT("/Script/GameplayAbilities"), TEXT("UAttributeSet"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAttributeSet);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
