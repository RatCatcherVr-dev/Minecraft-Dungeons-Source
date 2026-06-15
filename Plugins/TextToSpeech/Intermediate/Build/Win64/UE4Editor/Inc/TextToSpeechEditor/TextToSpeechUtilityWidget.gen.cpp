// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "TextToSpeechEditor/Public/TextToSpeechUtilityWidget.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTextToSpeechUtilityWidget() {}
// Cross Module References
	TEXTTOSPEECHEDITOR_API UClass* Z_Construct_UClass_UTextToSpeechUtilityWidget_NoRegister();
	TEXTTOSPEECHEDITOR_API UClass* Z_Construct_UClass_UTextToSpeechUtilityWidget();
	BLUTILITY_API UClass* Z_Construct_UClass_UEditorUtilityWidget();
	UPackage* Z_Construct_UPackage__Script_TextToSpeechEditor();
	TEXTTOSPEECHEDITOR_API UFunction* Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent();
// End Cross Module References
	static FName NAME_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent = FName(TEXT("ReceiveTextToSpeechEvent"));
	void UTextToSpeechUtilityWidget::ReceiveTextToSpeechEvent(const FString& inEvent, const FString& inOptionalData)
	{
		TextToSpeechUtilityWidget_eventReceiveTextToSpeechEvent_Parms Parms;
		Parms.inEvent=inEvent;
		Parms.inOptionalData=inOptionalData;
		ProcessEvent(FindFunctionChecked(NAME_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent),&Parms);
	}
	void UTextToSpeechUtilityWidget::StaticRegisterNativesUTextToSpeechUtilityWidget()
	{
	}
	struct Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_inOptionalData_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_inOptionalData;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_inEvent_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_inEvent;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inOptionalData_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inOptionalData = { "inOptionalData", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(TextToSpeechUtilityWidget_eventReceiveTextToSpeechEvent_Parms, inOptionalData), METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inOptionalData_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inOptionalData_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inEvent_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inEvent = { "inEvent", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(TextToSpeechUtilityWidget_eventReceiveTextToSpeechEvent_Parms, inEvent), METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inEvent_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inEvent_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inOptionalData,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::NewProp_inEvent,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::Function_MetaDataParams[] = {
		{ "Category", "Text To Speech Editor" },
		{ "DisplayName", "On Text To Speech Event" },
		{ "ModuleRelativePath", "Public/TextToSpeechUtilityWidget.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechUtilityWidget, nullptr, "ReceiveTextToSpeechEvent", sizeof(TextToSpeechUtilityWidget_eventReceiveTextToSpeechEvent_Parms), Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020800, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UTextToSpeechUtilityWidget_NoRegister()
	{
		return UTextToSpeechUtilityWidget::StaticClass();
	}
	struct Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UEditorUtilityWidget,
		(UObject* (*)())Z_Construct_UPackage__Script_TextToSpeechEditor,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UTextToSpeechUtilityWidget_ReceiveTextToSpeechEvent, "ReceiveTextToSpeechEvent" }, // 1085195989
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "TextToSpeechUtilityWidget.h" },
		{ "ModuleRelativePath", "Public/TextToSpeechUtilityWidget.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTextToSpeechUtilityWidget>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::ClassParams = {
		&UTextToSpeechUtilityWidget::StaticClass,
		"Editor",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x00B010A4u,
		METADATA_PARAMS(Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UTextToSpeechUtilityWidget()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UTextToSpeechUtilityWidget_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UTextToSpeechUtilityWidget, 3921590937);
	template<> TEXTTOSPEECHEDITOR_API UClass* StaticClass<UTextToSpeechUtilityWidget>()
	{
		return UTextToSpeechUtilityWidget::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UTextToSpeechUtilityWidget(Z_Construct_UClass_UTextToSpeechUtilityWidget, &UTextToSpeechUtilityWidget::StaticClass, TEXT("/Script/TextToSpeechEditor"), TEXT("UTextToSpeechUtilityWidget"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UTextToSpeechUtilityWidget);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
