// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "DungeonsLiveOps/Public/ConnectionStatus.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeConnectionStatus() {}
// Cross Module References
	DUNGEONSLIVEOPS_API UEnum* Z_Construct_UEnum_DungeonsLiveOps_EMinecraftAPIConnectionStatus();
	UPackage* Z_Construct_UPackage__Script_DungeonsLiveOps();
// End Cross Module References
	static UEnum* EMinecraftAPIConnectionStatus_StaticEnum()
	{
		static UEnum* Singleton = nullptr;
		if (!Singleton)
		{
			Singleton = GetStaticEnum(Z_Construct_UEnum_DungeonsLiveOps_EMinecraftAPIConnectionStatus, Z_Construct_UPackage__Script_DungeonsLiveOps(), TEXT("EMinecraftAPIConnectionStatus"));
		}
		return Singleton;
	}
	template<> DUNGEONSLIVEOPS_API UEnum* StaticEnum<EMinecraftAPIConnectionStatus>()
	{
		return EMinecraftAPIConnectionStatus_StaticEnum();
	}
	static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_EMinecraftAPIConnectionStatus(EMinecraftAPIConnectionStatus_StaticEnum, TEXT("/Script/DungeonsLiveOps"), TEXT("EMinecraftAPIConnectionStatus"), false, nullptr, nullptr);
	uint32 Get_Z_Construct_UEnum_DungeonsLiveOps_EMinecraftAPIConnectionStatus_Hash() { return 1643396662U; }
	UEnum* Z_Construct_UEnum_DungeonsLiveOps_EMinecraftAPIConnectionStatus()
	{
#if WITH_HOT_RELOAD
		UPackage* Outer = Z_Construct_UPackage__Script_DungeonsLiveOps();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("EMinecraftAPIConnectionStatus"), 0, Get_Z_Construct_UEnum_DungeonsLiveOps_EMinecraftAPIConnectionStatus_Hash(), false);
#else
		static UEnum* ReturnEnum = nullptr;
#endif // WITH_HOT_RELOAD
		if (!ReturnEnum)
		{
			static const UE4CodeGen_Private::FEnumeratorParam Enumerators[] = {
				{ "EMinecraftAPIConnectionStatus::Connected", (int64)EMinecraftAPIConnectionStatus::Connected },
				{ "EMinecraftAPIConnectionStatus::TimingOut", (int64)EMinecraftAPIConnectionStatus::TimingOut },
				{ "EMinecraftAPIConnectionStatus::NoConnection", (int64)EMinecraftAPIConnectionStatus::NoConnection },
				{ "EMinecraftAPIConnectionStatus::GameClientTooOld", (int64)EMinecraftAPIConnectionStatus::GameClientTooOld },
			};
#if WITH_METADATA
			const UE4CodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
				{ "BlueprintType", "true" },
				{ "ModuleRelativePath", "Public/ConnectionStatus.h" },
			};
#endif
			static const UE4CodeGen_Private::FEnumParams EnumParams = {
				(UObject*(*)())Z_Construct_UPackage__Script_DungeonsLiveOps,
				nullptr,
				"EMinecraftAPIConnectionStatus",
				"EMinecraftAPIConnectionStatus",
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
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
