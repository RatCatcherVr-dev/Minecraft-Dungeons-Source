//------------------------------------------------------------------------------
// <copyright file="UnrealToolsFrameworkPCH.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

/** Unreal Engine includes. */
#include "Engine.h"

/**
 * Guard used to enable TDK RPC (under third party) and hook
 * functionality only on development builds.
 */
#if (!UE_BUILD_SHIPPING) && (defined(PLATFORM_WINDOWS) || defined(PLATFORM_XBOXONE))
#   define ENABLE_GAMESTEST_RPC
#endif

#ifndef MAKE_HRESULT
#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#endif

#ifndef SEVERITY_SUCCESS
#define SEVERITY_SUCCESS 0
#endif

#ifndef SEVERITY_ERROR
#define SEVERITY_ERROR 1
#endif

/** Public includes of the Unreal Tools Framework. */
#include "UTFAnnotations.h"
#include "UnrealToolsFramework.h"
#include "UTFReturnCodes.h"
#include "Hooks/UTFHookManager.h"
#include "Hooks/Util/RpcActor.h"
#include "Hooks/Util/RpcArchiveExtensions.h"
#include "Hooks/Util/RpcBoundingBox.h"
#include "Hooks/Util/RpcLocalPlayer.h"
#include "Hooks/Util/RpcPawn.h"
#include "Hooks/Util/RpcRotator.h"
#include "Hooks/Util/RpcVector.h"
#include "Util/UnrealEngineHelperFunctions.h"
