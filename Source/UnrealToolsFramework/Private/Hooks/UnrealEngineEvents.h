//------------------------------------------------------------------------------
// <copyright file="UnrealEngineEvents.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

/**
 * Register a series of common Unreal Engine events.
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterEngineEvents();

/** Event triggered when a new map has been loaded. */
void PostLoadMap(UWorld* World);

}}}}

#endif // ENABLE_GAMESTEST_RPC
