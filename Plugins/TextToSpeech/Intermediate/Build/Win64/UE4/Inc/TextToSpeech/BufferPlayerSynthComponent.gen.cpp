// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "TextToSpeech/Private/Flite/BufferPlayerSynthComponent.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeBufferPlayerSynthComponent() {}
// Cross Module References
	TEXTTOSPEECH_API UClass* Z_Construct_UClass_UBufferPlayerSynthComponent_NoRegister();
	TEXTTOSPEECH_API UClass* Z_Construct_UClass_UBufferPlayerSynthComponent();
	AUDIOMIXER_API UClass* Z_Construct_UClass_USynthComponent();
	UPackage* Z_Construct_UPackage__Script_TextToSpeech();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished();
// End Cross Module References
	void UBufferPlayerSynthComponent::StaticRegisterNativesUBufferPlayerSynthComponent()
	{
		UClass* Class = UBufferPlayerSynthComponent::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "GetPlaybackPercentage", &UBufferPlayerSynthComponent::execGetPlaybackPercentage },
			{ "IsFinished", &UBufferPlayerSynthComponent::execIsFinished },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics
	{
		struct BufferPlayerSynthComponent_eventGetPlaybackPercentage_Parms
		{
			float ReturnValue;
		};
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(BufferPlayerSynthComponent_eventGetPlaybackPercentage_Parms, ReturnValue), METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::Function_MetaDataParams[] = {
		{ "Category", "Synth|BufferReader" },
		{ "ModuleRelativePath", "Private/Flite/BufferPlayerSynthComponent.h" },
		{ "ToolTip", "Returns the playback percentage of the buffer reader" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UBufferPlayerSynthComponent, nullptr, "GetPlaybackPercentage", sizeof(BufferPlayerSynthComponent_eventGetPlaybackPercentage_Parms), Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics
	{
		struct BufferPlayerSynthComponent_eventIsFinished_Parms
		{
			bool ReturnValue;
		};
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((BufferPlayerSynthComponent_eventIsFinished_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(BufferPlayerSynthComponent_eventIsFinished_Parms), &Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::Function_MetaDataParams[] = {
		{ "Category", "Synth|BufferReader" },
		{ "ModuleRelativePath", "Private/Flite/BufferPlayerSynthComponent.h" },
		{ "ToolTip", "Returns if the buffer reader has finished playing" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UBufferPlayerSynthComponent, nullptr, "IsFinished", sizeof(BufferPlayerSynthComponent_eventIsFinished_Parms), Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UBufferPlayerSynthComponent_NoRegister()
	{
		return UBufferPlayerSynthComponent::StaticClass();
	}
	struct Z_Construct_UClass_UBufferPlayerSynthComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_USynthComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_TextToSpeech,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UBufferPlayerSynthComponent_GetPlaybackPercentage, "GetPlaybackPercentage" }, // 281965841
		{ &Z_Construct_UFunction_UBufferPlayerSynthComponent_IsFinished, "IsFinished" }, // 605898273
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Synth" },
		{ "HideCategories", "Object ActorComponent Physics Rendering Mobility LOD Trigger PhysicsVolume" },
		{ "IncludePath", "Flite/BufferPlayerSynthComponent.h" },
		{ "ModuleRelativePath", "Private/Flite/BufferPlayerSynthComponent.h" },
		{ "ToolTip", "UBufferReaderSynthComponent" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBufferPlayerSynthComponent>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::ClassParams = {
		&UBufferPlayerSynthComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UBufferPlayerSynthComponent()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UBufferPlayerSynthComponent_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UBufferPlayerSynthComponent, 3673823386);
	template<> TEXTTOSPEECH_API UClass* StaticClass<UBufferPlayerSynthComponent>()
	{
		return UBufferPlayerSynthComponent::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UBufferPlayerSynthComponent(Z_Construct_UClass_UBufferPlayerSynthComponent, &UBufferPlayerSynthComponent::StaticClass, TEXT("/Script/TextToSpeech"), TEXT("UBufferPlayerSynthComponent"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UBufferPlayerSynthComponent);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
