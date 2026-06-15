// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FTextToPlay;
#ifdef TEXTTOSPEECH_TextToSpeechBPLibrary_generated_h
#error "TextToSpeechBPLibrary.generated.h already included, missing '#pragma once' in TextToSpeechBPLibrary.h"
#endif
#define TEXTTOSPEECH_TextToSpeechBPLibrary_generated_h

#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_25_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FTextToPlay_Statics; \
	TEXTTOSPEECH_API static class UScriptStruct* StaticStruct();


template<> TEXTTOSPEECH_API UScriptStruct* StaticStruct<struct FTextToPlay>();

#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execIsTextToSpeechSupported) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::IsTextToSpeechSupported(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCanInterrupt) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::GetCanInterrupt(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetStartDelayCurrent) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UTextToSpeechBPLibrary::GetStartDelayCurrent(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetStartDelayMax) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UTextToSpeechBPLibrary::GetStartDelayMax(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetLastPlayed) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTextToPlay*)Z_Param__Result=UTextToSpeechBPLibrary::GetLastPlayed(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTextQueue) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FTextToPlay>*)Z_Param__Result=UTextToSpeechBPLibrary::GetTextQueue(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTextToSpeechEnabled) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::GetTextToSpeechEnabled(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsTextToSpeechPlaying) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::IsTextToSpeechPlaying(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execPrioritiseLatestTextToSpeech) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::PrioritiseLatestTextToSpeech(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSkipTextToSpeech) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::SkipTextToSpeech(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execStopTextToSpeech) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::StopTextToSpeech(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execPlayTextToSpeech) \
	{ \
		P_GET_PROPERTY_REF(UTextProperty,Z_Param_Out_text); \
		P_GET_UBOOL(Z_Param_bMustFinishPlaying); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::PlayTextToSpeech(Z_Param_Out_text,Z_Param_bMustFinishPlaying); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetTextToSpeechEnabled) \
	{ \
		P_GET_UBOOL(Z_Param_bEnabled); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::SetTextToSpeechEnabled(Z_Param_bEnabled); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execIsTextToSpeechSupported) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::IsTextToSpeechSupported(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetCanInterrupt) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::GetCanInterrupt(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetStartDelayCurrent) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UTextToSpeechBPLibrary::GetStartDelayCurrent(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetStartDelayMax) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UTextToSpeechBPLibrary::GetStartDelayMax(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetLastPlayed) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(FTextToPlay*)Z_Param__Result=UTextToSpeechBPLibrary::GetLastPlayed(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTextQueue) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(TArray<FTextToPlay>*)Z_Param__Result=UTextToSpeechBPLibrary::GetTextQueue(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execGetTextToSpeechEnabled) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::GetTextToSpeechEnabled(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsTextToSpeechPlaying) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=UTextToSpeechBPLibrary::IsTextToSpeechPlaying(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execPrioritiseLatestTextToSpeech) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::PrioritiseLatestTextToSpeech(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSkipTextToSpeech) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::SkipTextToSpeech(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execStopTextToSpeech) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::StopTextToSpeech(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execPlayTextToSpeech) \
	{ \
		P_GET_PROPERTY_REF(UTextProperty,Z_Param_Out_text); \
		P_GET_UBOOL(Z_Param_bMustFinishPlaying); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::PlayTextToSpeech(Z_Param_Out_text,Z_Param_bMustFinishPlaying); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetTextToSpeechEnabled) \
	{ \
		P_GET_UBOOL(Z_Param_bEnabled); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		UTextToSpeechBPLibrary::SetTextToSpeechEnabled(Z_Param_bEnabled); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUTextToSpeechBPLibrary(); \
	friend struct Z_Construct_UClass_UTextToSpeechBPLibrary_Statics; \
public: \
	DECLARE_CLASS(UTextToSpeechBPLibrary, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/TextToSpeech"), NO_API) \
	DECLARE_SERIALIZER(UTextToSpeechBPLibrary)


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_INCLASS \
private: \
	static void StaticRegisterNativesUTextToSpeechBPLibrary(); \
	friend struct Z_Construct_UClass_UTextToSpeechBPLibrary_Statics; \
public: \
	DECLARE_CLASS(UTextToSpeechBPLibrary, UObject, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/TextToSpeech"), NO_API) \
	DECLARE_SERIALIZER(UTextToSpeechBPLibrary)


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UTextToSpeechBPLibrary(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UTextToSpeechBPLibrary) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UTextToSpeechBPLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UTextToSpeechBPLibrary); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UTextToSpeechBPLibrary(UTextToSpeechBPLibrary&&); \
	NO_API UTextToSpeechBPLibrary(const UTextToSpeechBPLibrary&); \
public:


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UTextToSpeechBPLibrary(UTextToSpeechBPLibrary&&); \
	NO_API UTextToSpeechBPLibrary(const UTextToSpeechBPLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UTextToSpeechBPLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UTextToSpeechBPLibrary); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UTextToSpeechBPLibrary)


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_38_PROLOG
#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_INCLASS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h_41_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> TEXTTOSPEECH_API UClass* StaticClass<class UTextToSpeechBPLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Public_TextToSpeechBPLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
