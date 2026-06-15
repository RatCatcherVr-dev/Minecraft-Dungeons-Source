// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef TEXTTOSPEECH_BufferPlayerSynthComponent_generated_h
#error "BufferPlayerSynthComponent.generated.h already included, missing '#pragma once' in BufferPlayerSynthComponent.h"
#endif
#define TEXTTOSPEECH_BufferPlayerSynthComponent_generated_h

#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execGetPlaybackPercentage) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->GetPlaybackPercentage(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsFinished) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsFinished(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execGetPlaybackPercentage) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=P_THIS->GetPlaybackPercentage(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsFinished) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsFinished(); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUBufferPlayerSynthComponent(); \
	friend struct Z_Construct_UClass_UBufferPlayerSynthComponent_Statics; \
public: \
	DECLARE_CLASS(UBufferPlayerSynthComponent, USynthComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/TextToSpeech"), NO_API) \
	DECLARE_SERIALIZER(UBufferPlayerSynthComponent)


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_INCLASS \
private: \
	static void StaticRegisterNativesUBufferPlayerSynthComponent(); \
	friend struct Z_Construct_UClass_UBufferPlayerSynthComponent_Statics; \
public: \
	DECLARE_CLASS(UBufferPlayerSynthComponent, USynthComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/TextToSpeech"), NO_API) \
	DECLARE_SERIALIZER(UBufferPlayerSynthComponent)


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UBufferPlayerSynthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UBufferPlayerSynthComponent) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBufferPlayerSynthComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBufferPlayerSynthComponent); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UBufferPlayerSynthComponent(UBufferPlayerSynthComponent&&); \
	NO_API UBufferPlayerSynthComponent(const UBufferPlayerSynthComponent&); \
public:


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UBufferPlayerSynthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UBufferPlayerSynthComponent(UBufferPlayerSynthComponent&&); \
	NO_API UBufferPlayerSynthComponent(const UBufferPlayerSynthComponent&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBufferPlayerSynthComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBufferPlayerSynthComponent); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UBufferPlayerSynthComponent)


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_10_PROLOG
#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_INCLASS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> TEXTTOSPEECH_API UClass* StaticClass<class UBufferPlayerSynthComponent>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_TextToSpeech_Source_TextToSpeech_Private_Flite_BufferPlayerSynthComponent_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
