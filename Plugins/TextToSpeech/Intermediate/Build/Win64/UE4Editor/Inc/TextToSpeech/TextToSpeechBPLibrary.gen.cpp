// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "TextToSpeech/Public/TextToSpeechBPLibrary.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTextToSpeechBPLibrary() {}
// Cross Module References
	TEXTTOSPEECH_API UScriptStruct* Z_Construct_UScriptStruct_FTextToPlay();
	UPackage* Z_Construct_UPackage__Script_TextToSpeech();
	TEXTTOSPEECH_API UClass* Z_Construct_UClass_UTextToSpeechBPLibrary_NoRegister();
	TEXTTOSPEECH_API UClass* Z_Construct_UClass_UTextToSpeechBPLibrary();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech();
	TEXTTOSPEECH_API UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech();
// End Cross Module References
class UScriptStruct* FTextToPlay::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern TEXTTOSPEECH_API uint32 Get_Z_Construct_UScriptStruct_FTextToPlay_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FTextToPlay, Z_Construct_UPackage__Script_TextToSpeech(), TEXT("TextToPlay"), sizeof(FTextToPlay), Get_Z_Construct_UScriptStruct_FTextToPlay_Hash());
	}
	return Singleton;
}
template<> TEXTTOSPEECH_API UScriptStruct* StaticStruct<FTextToPlay>()
{
	return FTextToPlay::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FTextToPlay(FTextToPlay::StaticStruct, TEXT("/Script/TextToSpeech"), TEXT("TextToPlay"), false, nullptr, nullptr);
static struct FScriptStruct_TextToSpeech_StaticRegisterNativesFTextToPlay
{
	FScriptStruct_TextToSpeech_StaticRegisterNativesFTextToPlay()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("TextToPlay")),new UScriptStruct::TCppStructOps<FTextToPlay>);
	}
} ScriptStruct_TextToSpeech_StaticRegisterNativesFTextToPlay;
	struct Z_Construct_UScriptStruct_FTextToPlay_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MustFinishPlaying_MetaData[];
#endif
		static void NewProp_MustFinishPlaying_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_MustFinishPlaying;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Text_MetaData[];
#endif
		static const UE4CodeGen_Private::FTextPropertyParams NewProp_Text;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FTextToPlay_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FTextToPlay_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FTextToPlay>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_MustFinishPlaying_MetaData[] = {
		{ "Category", "TextToPlay" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	void Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_MustFinishPlaying_SetBit(void* Obj)
	{
		((FTextToPlay*)Obj)->MustFinishPlaying = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_MustFinishPlaying = { "MustFinishPlaying", nullptr, (EPropertyFlags)0x0010000000000004, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(FTextToPlay), &Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_MustFinishPlaying_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_MustFinishPlaying_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_MustFinishPlaying_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_Text_MetaData[] = {
		{ "Category", "TextToPlay" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FTextPropertyParams Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_Text = { "Text", nullptr, (EPropertyFlags)0x0010000000000004, UE4CodeGen_Private::EPropertyGenFlags::Text, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FTextToPlay, Text), METADATA_PARAMS(Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_Text_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_Text_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FTextToPlay_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_MustFinishPlaying,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FTextToPlay_Statics::NewProp_Text,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FTextToPlay_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_TextToSpeech,
		nullptr,
		&NewStructOps,
		"TextToPlay",
		sizeof(FTextToPlay),
		alignof(FTextToPlay),
		Z_Construct_UScriptStruct_FTextToPlay_Statics::PropPointers,
		ARRAY_COUNT(Z_Construct_UScriptStruct_FTextToPlay_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FTextToPlay_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FTextToPlay_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FTextToPlay()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FTextToPlay_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_TextToSpeech();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("TextToPlay"), sizeof(FTextToPlay), Get_Z_Construct_UScriptStruct_FTextToPlay_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FTextToPlay_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FTextToPlay_Hash() { return 2916669490U; }
	void UTextToSpeechBPLibrary::StaticRegisterNativesUTextToSpeechBPLibrary()
	{
		UClass* Class = UTextToSpeechBPLibrary::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "GetCanInterrupt", &UTextToSpeechBPLibrary::execGetCanInterrupt },
			{ "GetLastPlayed", &UTextToSpeechBPLibrary::execGetLastPlayed },
			{ "GetStartDelayCurrent", &UTextToSpeechBPLibrary::execGetStartDelayCurrent },
			{ "GetStartDelayMax", &UTextToSpeechBPLibrary::execGetStartDelayMax },
			{ "GetTextQueue", &UTextToSpeechBPLibrary::execGetTextQueue },
			{ "GetTextToSpeechEnabled", &UTextToSpeechBPLibrary::execGetTextToSpeechEnabled },
			{ "IsTextToSpeechPlaying", &UTextToSpeechBPLibrary::execIsTextToSpeechPlaying },
			{ "IsTextToSpeechSupported", &UTextToSpeechBPLibrary::execIsTextToSpeechSupported },
			{ "PlayTextToSpeech", &UTextToSpeechBPLibrary::execPlayTextToSpeech },
			{ "PrioritiseLatestTextToSpeech", &UTextToSpeechBPLibrary::execPrioritiseLatestTextToSpeech },
			{ "SetTextToSpeechEnabled", &UTextToSpeechBPLibrary::execSetTextToSpeechEnabled },
			{ "SkipTextToSpeech", &UTextToSpeechBPLibrary::execSkipTextToSpeech },
			{ "StopTextToSpeech", &UTextToSpeechBPLibrary::execStopTextToSpeech },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics
	{
		struct TextToSpeechBPLibrary_eventGetCanInterrupt_Parms
		{
			bool ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	void Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((TextToSpeechBPLibrary_eventGetCanInterrupt_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000582, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(TextToSpeechBPLibrary_eventGetCanInterrupt_Parms), &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Get Text To Speech Can Interrupt" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "GetCanInterrupt", sizeof(TextToSpeechBPLibrary_eventGetCanInterrupt_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics
	{
		struct TextToSpeechBPLibrary_eventGetLastPlayed_Parms
		{
			FTextToPlay ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000582, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(TextToSpeechBPLibrary_eventGetLastPlayed_Parms, ReturnValue), Z_Construct_UScriptStruct_FTextToPlay, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Get Text To Speech Last Played" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "GetLastPlayed", sizeof(TextToSpeechBPLibrary_eventGetLastPlayed_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics
	{
		struct TextToSpeechBPLibrary_eventGetStartDelayCurrent_Parms
		{
			float ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000582, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(TextToSpeechBPLibrary_eventGetStartDelayCurrent_Parms, ReturnValue), METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Get Text To Speech Start Delay Current" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "GetStartDelayCurrent", sizeof(TextToSpeechBPLibrary_eventGetStartDelayCurrent_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics
	{
		struct TextToSpeechBPLibrary_eventGetStartDelayMax_Parms
		{
			float ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000582, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(TextToSpeechBPLibrary_eventGetStartDelayMax_Parms, ReturnValue), METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Get Text To Speech Start Delay Max" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "GetStartDelayMax", sizeof(TextToSpeechBPLibrary_eventGetStartDelayMax_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics
	{
		struct TextToSpeechBPLibrary_eventGetTextQueue_Parms
		{
			TArray<FTextToPlay> ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_ReturnValue_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000008000582, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(TextToSpeechBPLibrary_eventGetTextQueue_Parms, ReturnValue), METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FTextToPlay, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::NewProp_ReturnValue,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::NewProp_ReturnValue_Inner,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Get Text To Speech Queue" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "GetTextQueue", sizeof(TextToSpeechBPLibrary_eventGetTextQueue_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics
	{
		struct TextToSpeechBPLibrary_eventGetTextToSpeechEnabled_Parms
		{
			bool ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	void Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((TextToSpeechBPLibrary_eventGetTextToSpeechEnabled_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000582, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(TextToSpeechBPLibrary_eventGetTextToSpeechEnabled_Parms), &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Get Text To Speech Enabled" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "GetTextToSpeechEnabled", sizeof(TextToSpeechBPLibrary_eventGetTextToSpeechEnabled_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics
	{
		struct TextToSpeechBPLibrary_eventIsTextToSpeechPlaying_Parms
		{
			bool ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	void Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((TextToSpeechBPLibrary_eventIsTextToSpeechPlaying_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000582, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(TextToSpeechBPLibrary_eventIsTextToSpeechPlaying_Parms), &Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Is Text To Speech Playing" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "IsTextToSpeechPlaying", sizeof(TextToSpeechBPLibrary_eventIsTextToSpeechPlaying_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics
	{
		struct TextToSpeechBPLibrary_eventIsTextToSpeechSupported_Parms
		{
			bool ReturnValue;
		};
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	void Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((TextToSpeechBPLibrary_eventIsTextToSpeechSupported_Parms*)Obj)->ReturnValue = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000582, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(TextToSpeechBPLibrary_eventIsTextToSpeechSupported_Parms), &Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::NewProp_ReturnValue_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::NewProp_ReturnValue_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Is Text To Speech Supported" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "IsTextToSpeechSupported", sizeof(TextToSpeechBPLibrary_eventIsTextToSpeechSupported_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics
	{
		struct TextToSpeechBPLibrary_eventPlayTextToSpeech_Parms
		{
			FText text;
			bool bMustFinishPlaying;
		};
		static void NewProp_bMustFinishPlaying_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMustFinishPlaying;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_text_MetaData[];
#endif
		static const UE4CodeGen_Private::FTextPropertyParams NewProp_text;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_bMustFinishPlaying_SetBit(void* Obj)
	{
		((TextToSpeechBPLibrary_eventPlayTextToSpeech_Parms*)Obj)->bMustFinishPlaying = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_bMustFinishPlaying = { "bMustFinishPlaying", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(TextToSpeechBPLibrary_eventPlayTextToSpeech_Parms), &Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_bMustFinishPlaying_SetBit, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_text_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif
	const UE4CodeGen_Private::FTextPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_text = { "text", nullptr, (EPropertyFlags)0x0010000008000182, UE4CodeGen_Private::EPropertyGenFlags::Text, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(TextToSpeechBPLibrary_eventPlayTextToSpeech_Parms, text), METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_text_MetaData, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_text_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_bMustFinishPlaying,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::NewProp_text,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::Function_MetaDataParams[] = {
		{ "AutoCreateRefTerm", "Text" },
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Play Text To Speech" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
		{ "ToolTip", "Creates a Text To Speech request and appends it to the queue" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "PlayTextToSpeech", sizeof(TextToSpeechBPLibrary_eventPlayTextToSpeech_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04422401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Prioritise Latest Text To Speech" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "PrioritiseLatestTextToSpeech", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics
	{
		struct TextToSpeechBPLibrary_eventSetTextToSpeechEnabled_Parms
		{
			bool bEnabled;
		};
		static void NewProp_bEnabled_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bEnabled;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::NewProp_bEnabled_SetBit(void* Obj)
	{
		((TextToSpeechBPLibrary_eventSetTextToSpeechEnabled_Parms*)Obj)->bEnabled = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::NewProp_bEnabled = { "bEnabled", nullptr, (EPropertyFlags)0x0010000000000080, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(TextToSpeechBPLibrary_eventSetTextToSpeechEnabled_Parms), &Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::NewProp_bEnabled_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::NewProp_bEnabled,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Set Text To Speech Enabled" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
		{ "ToolTip", "Enables or Disables Text To Speech" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "SetTextToSpeechEnabled", sizeof(TextToSpeechBPLibrary_eventSetTextToSpeechEnabled_Parms), Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Skip Text To Speech" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
		{ "ToolTip", "Skips Text To Speech playback if it is skippable. NOTE: This will not skip any phrases that must finish playing, and will not empty the queue" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "SkipTextToSpeech", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech_Statics::Function_MetaDataParams[] = {
		{ "Category", "TextToSpeech" },
		{ "DisplayName", "Stop Text To Speech" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
		{ "ToolTip", "Stops Text To Speech playback and empties the queue. NOTE: Since this empties the queue, all Text To Speech triggers up to that point will be lost" },
	};
#endif
	const UE4CodeGen_Private::FFunctionParams Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTextToSpeechBPLibrary, nullptr, "StopTextToSpeech", 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech_Statics::Function_MetaDataParams, ARRAY_COUNT(Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_UTextToSpeechBPLibrary_NoRegister()
	{
		return UTextToSpeechBPLibrary::StaticClass();
	}
	struct Z_Construct_UClass_UTextToSpeechBPLibrary_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_TextToSpeech,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetCanInterrupt, "GetCanInterrupt" }, // 3147197895
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetLastPlayed, "GetLastPlayed" }, // 3701166445
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayCurrent, "GetStartDelayCurrent" }, // 3155434691
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetStartDelayMax, "GetStartDelayMax" }, // 1104677783
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextQueue, "GetTextQueue" }, // 2938821052
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_GetTextToSpeechEnabled, "GetTextToSpeechEnabled" }, // 2623354497
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechPlaying, "IsTextToSpeechPlaying" }, // 2964843372
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_IsTextToSpeechSupported, "IsTextToSpeechSupported" }, // 3105846424
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_PlayTextToSpeech, "PlayTextToSpeech" }, // 1821100957
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_PrioritiseLatestTextToSpeech, "PrioritiseLatestTextToSpeech" }, // 3495000575
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_SetTextToSpeechEnabled, "SetTextToSpeechEnabled" }, // 3108401289
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_SkipTextToSpeech, "SkipTextToSpeech" }, // 444064136
		{ &Z_Construct_UFunction_UTextToSpeechBPLibrary_StopTextToSpeech, "StopTextToSpeech" }, // 179761651
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "TextToSpeechBPLibrary.h" },
		{ "ModuleRelativePath", "Public/TextToSpeechBPLibrary.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTextToSpeechBPLibrary>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::ClassParams = {
		&UTextToSpeechBPLibrary::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UTextToSpeechBPLibrary()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UTextToSpeechBPLibrary_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UTextToSpeechBPLibrary, 113843324);
	template<> TEXTTOSPEECH_API UClass* StaticClass<UTextToSpeechBPLibrary>()
	{
		return UTextToSpeechBPLibrary::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UTextToSpeechBPLibrary(Z_Construct_UClass_UTextToSpeechBPLibrary, &UTextToSpeechBPLibrary::StaticClass, TEXT("/Script/TextToSpeech"), TEXT("UTextToSpeechBPLibrary"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UTextToSpeechBPLibrary);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
