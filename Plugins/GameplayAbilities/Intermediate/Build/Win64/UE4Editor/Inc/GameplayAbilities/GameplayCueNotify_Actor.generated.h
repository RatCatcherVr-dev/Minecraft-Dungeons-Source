// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
struct FGameplayCueParameters;
#ifdef GAMEPLAYABILITIES_GameplayCueNotify_Actor_generated_h
#error "GameplayCueNotify_Actor.generated.h already included, missing '#pragma once' in GameplayCueNotify_Actor.h"
#endif
#define GAMEPLAYABILITIES_GameplayCueNotify_Actor_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_RPC_WRAPPERS \
	virtual bool OnRemove_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
	virtual bool WhileActive_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
	virtual bool OnActive_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
	virtual bool OnExecute_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
 \
	DECLARE_FUNCTION(execOnRemove) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->OnRemove_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWhileActive) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->WhileActive_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnActive) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->OnActive_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnExecute) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->OnExecute_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_EndGameplayCue) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_EndGameplayCue(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnOwnerDestroyed) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_DestroyedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnOwnerDestroyed(Z_Param_DestroyedActor); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual bool OnRemove_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
	virtual bool WhileActive_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
	virtual bool OnActive_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
	virtual bool OnExecute_Implementation(AActor* MyTarget, FGameplayCueParameters const& Parameters); \
 \
	DECLARE_FUNCTION(execOnRemove) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->OnRemove_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execWhileActive) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->WhileActive_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnActive) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->OnActive_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnExecute) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_MyTarget); \
		P_GET_STRUCT_REF(FGameplayCueParameters,Z_Param_Out_Parameters); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->OnExecute_Implementation(Z_Param_MyTarget,Z_Param_Out_Parameters); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execK2_EndGameplayCue) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->K2_EndGameplayCue(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnOwnerDestroyed) \
	{ \
		P_GET_OBJECT(AActor,Z_Param_DestroyedActor); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnOwnerDestroyed(Z_Param_DestroyedActor); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_EVENT_PARMS \
	struct GameplayCueNotify_Actor_eventK2_HandleGameplayCue_Parms \
	{ \
		AActor* MyTarget; \
		TEnumAsByte<EGameplayCueEvent::Type> EventType; \
		FGameplayCueParameters Parameters; \
	}; \
	struct GameplayCueNotify_Actor_eventOnActive_Parms \
	{ \
		AActor* MyTarget; \
		FGameplayCueParameters Parameters; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayCueNotify_Actor_eventOnActive_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	}; \
	struct GameplayCueNotify_Actor_eventOnExecute_Parms \
	{ \
		AActor* MyTarget; \
		FGameplayCueParameters Parameters; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayCueNotify_Actor_eventOnExecute_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	}; \
	struct GameplayCueNotify_Actor_eventOnRemove_Parms \
	{ \
		AActor* MyTarget; \
		FGameplayCueParameters Parameters; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayCueNotify_Actor_eventOnRemove_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	}; \
	struct GameplayCueNotify_Actor_eventWhileActive_Parms \
	{ \
		AActor* MyTarget; \
		FGameplayCueParameters Parameters; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayCueNotify_Actor_eventWhileActive_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	};


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_CALLBACK_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_ARCHIVESERIALIZER \
	DECLARE_FSTRUCTUREDARCHIVE_SERIALIZER(AGameplayCueNotify_Actor, NO_API)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAGameplayCueNotify_Actor(); \
	friend struct Z_Construct_UClass_AGameplayCueNotify_Actor_Statics; \
public: \
	DECLARE_CLASS(AGameplayCueNotify_Actor, AActor, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(AGameplayCueNotify_Actor) \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_ARCHIVESERIALIZER


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_INCLASS \
private: \
	static void StaticRegisterNativesAGameplayCueNotify_Actor(); \
	friend struct Z_Construct_UClass_AGameplayCueNotify_Actor_Statics; \
public: \
	DECLARE_CLASS(AGameplayCueNotify_Actor, AActor, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(AGameplayCueNotify_Actor) \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_ARCHIVESERIALIZER


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AGameplayCueNotify_Actor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AGameplayCueNotify_Actor) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AGameplayCueNotify_Actor); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AGameplayCueNotify_Actor); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AGameplayCueNotify_Actor(AGameplayCueNotify_Actor&&); \
	NO_API AGameplayCueNotify_Actor(const AGameplayCueNotify_Actor&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AGameplayCueNotify_Actor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AGameplayCueNotify_Actor(AGameplayCueNotify_Actor&&); \
	NO_API AGameplayCueNotify_Actor(const AGameplayCueNotify_Actor&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AGameplayCueNotify_Actor); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AGameplayCueNotify_Actor); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AGameplayCueNotify_Actor)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_17_PROLOG \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_EVENT_PARMS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h_20_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class GameplayCueNotify_Actor."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class AGameplayCueNotify_Actor>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayCueNotify_Actor_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
