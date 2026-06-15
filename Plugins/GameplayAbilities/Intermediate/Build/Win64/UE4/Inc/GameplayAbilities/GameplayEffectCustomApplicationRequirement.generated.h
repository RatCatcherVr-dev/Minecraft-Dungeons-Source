// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UGameplayEffect;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
#ifdef GAMEPLAYABILITIES_GameplayEffectCustomApplicationRequirement_generated_h
#error "GameplayEffectCustomApplicationRequirement.generated.h already included, missing '#pragma once' in GameplayEffectCustomApplicationRequirement.h"
#endif
#define GAMEPLAYABILITIES_GameplayEffectCustomApplicationRequirement_generated_h

#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_RPC_WRAPPERS \
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, FGameplayEffectSpec const& Spec, UAbilitySystemComponent* ASC) const; \
 \
	DECLARE_FUNCTION(execCanApplyGameplayEffect) \
	{ \
		P_GET_OBJECT(UGameplayEffect,Z_Param_GameplayEffect); \
		P_GET_STRUCT_REF(FGameplayEffectSpec,Z_Param_Out_Spec); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_ASC); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->CanApplyGameplayEffect_Implementation(Z_Param_GameplayEffect,Z_Param_Out_Spec,Z_Param_ASC); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, FGameplayEffectSpec const& Spec, UAbilitySystemComponent* ASC) const; \
 \
	DECLARE_FUNCTION(execCanApplyGameplayEffect) \
	{ \
		P_GET_OBJECT(UGameplayEffect,Z_Param_GameplayEffect); \
		P_GET_STRUCT_REF(FGameplayEffectSpec,Z_Param_Out_Spec); \
		P_GET_OBJECT(UAbilitySystemComponent,Z_Param_ASC); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->CanApplyGameplayEffect_Implementation(Z_Param_GameplayEffect,Z_Param_Out_Spec,Z_Param_ASC); \
		P_NATIVE_END; \
	}


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_EVENT_PARMS \
	struct GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms \
	{ \
		const UGameplayEffect* GameplayEffect; \
		FGameplayEffectSpec Spec; \
		UAbilitySystemComponent* ASC; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		GameplayEffectCustomApplicationRequirement_eventCanApplyGameplayEffect_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	};


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_CALLBACK_WRAPPERS
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGameplayEffectCustomApplicationRequirement(); \
	friend struct Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics; \
public: \
	DECLARE_CLASS(UGameplayEffectCustomApplicationRequirement, UObject, COMPILED_IN_FLAGS(CLASS_Abstract), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayEffectCustomApplicationRequirement)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_INCLASS \
private: \
	static void StaticRegisterNativesUGameplayEffectCustomApplicationRequirement(); \
	friend struct Z_Construct_UClass_UGameplayEffectCustomApplicationRequirement_Statics; \
public: \
	DECLARE_CLASS(UGameplayEffectCustomApplicationRequirement, UObject, COMPILED_IN_FLAGS(CLASS_Abstract), CASTCLASS_None, TEXT("/Script/GameplayAbilities"), NO_API) \
	DECLARE_SERIALIZER(UGameplayEffectCustomApplicationRequirement)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayEffectCustomApplicationRequirement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayEffectCustomApplicationRequirement) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayEffectCustomApplicationRequirement); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayEffectCustomApplicationRequirement); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayEffectCustomApplicationRequirement(UGameplayEffectCustomApplicationRequirement&&); \
	NO_API UGameplayEffectCustomApplicationRequirement(const UGameplayEffectCustomApplicationRequirement&); \
public:


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UGameplayEffectCustomApplicationRequirement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UGameplayEffectCustomApplicationRequirement(UGameplayEffectCustomApplicationRequirement&&); \
	NO_API UGameplayEffectCustomApplicationRequirement(const UGameplayEffectCustomApplicationRequirement&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGameplayEffectCustomApplicationRequirement); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGameplayEffectCustomApplicationRequirement); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UGameplayEffectCustomApplicationRequirement)


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_PRIVATE_PROPERTY_OFFSET
#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_15_PROLOG \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_EVENT_PARMS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_RPC_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_INCLASS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_PRIVATE_PROPERTY_OFFSET \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_RPC_WRAPPERS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_CALLBACK_WRAPPERS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_INCLASS_NO_PURE_DECLS \
	Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h_20_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class GameplayEffectCustomApplicationRequirement."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> GAMEPLAYABILITIES_API UClass* StaticClass<class UGameplayEffectCustomApplicationRequirement>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayEffectCustomApplicationRequirement_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
