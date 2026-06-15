// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef DUNGEONSLIVEOPS_ConnectionStatus_generated_h
#error "ConnectionStatus.generated.h already included, missing '#pragma once' in ConnectionStatus.h"
#endif
#define DUNGEONSLIVEOPS_ConnectionStatus_generated_h

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Minecraft_Dungeons_Source_Plugins_DungeonsAPIClient_Source_DungeonsLiveOps_Public_ConnectionStatus_h


#define FOREACH_ENUM_EMINECRAFTAPICONNECTIONSTATUS(op) \
	op(EMinecraftAPIConnectionStatus::Connected) \
	op(EMinecraftAPIConnectionStatus::TimingOut) \
	op(EMinecraftAPIConnectionStatus::NoConnection) \
	op(EMinecraftAPIConnectionStatus::GameClientTooOld) 

enum class EMinecraftAPIConnectionStatus : uint8;
template<> DUNGEONSLIVEOPS_API UEnum* StaticEnum<EMinecraftAPIConnectionStatus>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
