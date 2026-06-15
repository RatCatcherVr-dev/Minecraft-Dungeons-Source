// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilitiesEditor/Public/K2Node_GameplayCueEvent.h"
#include "Serialization/ArchiveUObjectFromStructuredArchive.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeK2Node_GameplayCueEvent() {}
// Cross Module References
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UK2Node_GameplayCueEvent_NoRegister();
	GAMEPLAYABILITIESEDITOR_API UClass* Z_Construct_UClass_UK2Node_GameplayCueEvent();
	BLUEPRINTGRAPH_API UClass* Z_Construct_UClass_UK2Node_Event();
	UPackage* Z_Construct_UPackage__Script_GameplayAbilitiesEditor();
// End Cross Module References
	void UK2Node_GameplayCueEvent::StaticRegisterNativesUK2Node_GameplayCueEvent()
	{
	}
	UClass* Z_Construct_UClass_UK2Node_GameplayCueEvent_NoRegister()
	{
		return UK2Node_GameplayCueEvent::StaticClass();
	}
	struct Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UK2Node_Event,
		(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilitiesEditor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "K2Node_GameplayCueEvent.h" },
		{ "ModuleRelativePath", "Public/K2Node_GameplayCueEvent.h" },
		{ "SerializeToFArchive", "" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UK2Node_GameplayCueEvent>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics::ClassParams = {
		&UK2Node_GameplayCueEvent::StaticClass,
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
		0x000000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UK2Node_GameplayCueEvent()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UK2Node_GameplayCueEvent_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UK2Node_GameplayCueEvent, 1769259952);
	template<> GAMEPLAYABILITIESEDITOR_API UClass* StaticClass<UK2Node_GameplayCueEvent>()
	{
		return UK2Node_GameplayCueEvent::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UK2Node_GameplayCueEvent(Z_Construct_UClass_UK2Node_GameplayCueEvent, &UK2Node_GameplayCueEvent::StaticClass, TEXT("/Script/GameplayAbilitiesEditor"), TEXT("UK2Node_GameplayCueEvent"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UK2Node_GameplayCueEvent);
	IMPLEMENT_FSTRUCTUREDARCHIVE_SERIALIZER(UK2Node_GameplayCueEvent)
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
